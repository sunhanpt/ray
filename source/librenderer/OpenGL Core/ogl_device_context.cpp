// +---------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "ogl_device_context.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_input_layout.h"
#include "ogl_sampler.h"
#include "ogl_descriptor_set.h"
#include "ogl_pipeline.h"
#include "ogl_swapchain.h"
#include "ogl_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(OGLDeviceContext, GraphicsContext, "OGLDeviceContext")

OGLDeviceContext::OGLDeviceContext() noexcept
	: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
	, _clearDepth(1.0f)
	, _clearStencil(0)
	, _globalVao(GL_NONE)
	, _framebuffer(nullptr)
	, _program(nullptr)
	, _pipeline(nullptr)
	, _descriptorSet(nullptr)
	, _indexOffset(0)
	, _indexType(GL_UNSIGNED_INT)
	, _state(nullptr)
	, _glcontext(nullptr)
	, _needUpdatePipeline(true)
	, _needUpdateDescriptor(true)
	, _needUpdateVertexBuffers(true)
{
	_vertexBuffers.resize(8);

	std::memset(&_viewport, 0, sizeof(_viewport));
	std::memset(&_scissor, 0, sizeof(_scissor));

	GraphicsColorBlends blends(4);
	_stateCaptured.setColorBlends(blends);
}

OGLDeviceContext::~OGLDeviceContext() noexcept
{
	this->close();
}

bool
OGLDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	assert(desc.getSwapchain());
	assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

	_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
	_glcontext->setActive(true);

	if (_glcontext->getActive())
	{
		if (!this->checkSupport())
			return false;

		if (!this->initDebugControl(desc))
			return false;

		if (!this->initStateSystem())
			return false;

		if (!this->initTextureSupports())
			return false;

		if (!this->initVertexSupports())
			return false;

		if (!this->initTextureDimSupports())
			return false;

		if (!this->initShaderSupports())
			return false;

		return true;
	}

	return false;
}

void
OGLDeviceContext::close() noexcept
{
	_framebuffer = nullptr;
	_program = nullptr;
	_pipeline = nullptr;
	_descriptorSet = nullptr;
	_state = nullptr;
	_glcontext = nullptr;
	_indexBuffer.reset();
	_vertexBuffers.clear();
	_supportTextures.clear();
	_supportAttribute.clear();

	if (_globalVao)
	{
		glDeleteVertexArrays(1, &_globalVao);
		_globalVao = GL_NONE;
	}
}

void
OGLDeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);
}

void
OGLDeviceContext::renderEnd() noexcept
{
}

void
OGLDeviceContext::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_glcontext->getActive());

	if (_viewport != view)
	{
		glViewport(view.left, view.top, view.width, view.height);
		_viewport = view;
	}
}

const Viewport&
OGLDeviceContext::getViewport(std::uint32_t i) const noexcept
{
	return _viewport;
}

void
OGLDeviceContext::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_glcontext->getActive());

	if (_scissor != scissor)
	{
		glScissor(scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor = scissor;
	}
}

const Scissor&
OGLDeviceContext::getScissor(std::uint32_t i) const noexcept
{
	return _scissor;
}
void
OGLDeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontReadMask() != mask)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilFrontReadMask(mask);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackReadMask() != mask)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask);
			_stateCaptured.setStencilBackReadMask(mask);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontReadMask();
	else
		return _stateCaptured.getStencilBackReadMask();
}

void
OGLDeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontRef() != reference)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontRef(reference);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackRef() != reference)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask());
			_stateCaptured.setStencilBackRef(reference);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilReference(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontRef();
	else
		return _stateCaptured.getStencilBackRef();
}

void
OGLDeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_FRONT, mask);
			_stateCaptured.setStencilFrontWriteMask(mask);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_BACK, mask);
			_stateCaptured.setStencilBackWriteMask(mask);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontWriteMask();
	else
		return _stateCaptured.getStencilBackWriteMask();
}

void
OGLDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<OGLPipeline>());
	assert(_glcontext->getActive());

	auto glpipeline = pipeline->downcast_pointer<OGLPipeline>();
	if (_pipeline != glpipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<OGLGraphicsState>();
		if (_state != glstate)
		{
			glstate->apply(_stateCaptured);
			_state = glstate;
		}

		auto glprogram = pipelineDesc.getGraphicsProgram()->downcast_pointer<OGLProgram>();
		if (_program != glprogram)
		{
			_program = glprogram;
			_program->apply();
		}

		if (_pipeline != glpipeline)
		{
			_pipeline = glpipeline;
			_pipeline->apply();
			_needUpdatePipeline = true;
		}
	}
}

GraphicsPipelinePtr
OGLDeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
OGLDeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<OGLDescriptorSet>());
	assert(_glcontext->getActive());

	auto glDescriptorSet = descriptorSet->downcast_pointer<OGLDescriptorSet>();
	if (_descriptorSet != glDescriptorSet)
	{
		_descriptorSet = glDescriptorSet;
		_needUpdateDescriptor = true;
	}
}

GraphicsDescriptorSetPtr
OGLDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
OGLDeviceContext::setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	assert(_vertexBuffers.size() > i);
	assert(_glcontext->getActive());

	auto vbo = data->downcast_pointer<OGLGraphicsData>();
	if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
	{
		_vertexBuffers[i].vbo = vbo;
		_vertexBuffers[i].offset = offset;
		_vertexBuffers[i].needUpdate = true;
		_needUpdateVertexBuffers = true;
	}
}

GraphicsDataPtr
OGLDeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
{
	assert(_vertexBuffers.size() > i);
	return _vertexBuffers[i].vbo;
}

void
OGLDeviceContext::setIndexBufferData(GraphicsDataPtr data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(indexType == GraphicsIndexType::GraphicsIndexTypeUInt16 || indexType == GraphicsIndexType::GraphicsIndexTypeUInt32);
	assert(_glcontext->getActive());

	auto ibo = data->downcast_pointer<OGLGraphicsData>();
	if (_indexBuffer != ibo)
	{
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
		_indexBuffer = ibo;
	}

	_indexOffset = offset;
	_indexType = OGLTypes::asIndexType(indexType);
}

GraphicsDataPtr
OGLDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

void 
OGLDeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipeline);
	assert(_glcontext->getActive());
	assert(startInstances == 0);

	if (_needUpdatePipeline || _needUpdateVertexBuffers)
	{
		_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
		_needUpdatePipeline = false;
		_needUpdateVertexBuffers = false;
	}

	if (_needUpdateDescriptor)
	{
		_descriptorSet->apply(*_program);
		_needUpdateDescriptor = false;
	}

	if (numVertices > 0)
	{
		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawArraysInstanced(drawType, startVertice, numVertices, numInstances);
	}
}

void 
OGLDeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipeline);
	assert(_glcontext->getActive());
	assert(_indexBuffer);
	assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
	assert(startInstances == 0);

	if (_needUpdatePipeline || _needUpdateVertexBuffers)
	{
		_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
		_needUpdatePipeline = false;
		_needUpdateVertexBuffers = false;
	}

	if (_needUpdateDescriptor)
	{
		_descriptorSet->apply(*_program);
		_needUpdateDescriptor = false;
	}

	if (numIndices > 0)
	{
		GLbyte* offsetIndices = nullptr;
		if (_indexType == GL_UNSIGNED_INT)
			offsetIndices = offsetIndices + _indexOffset + sizeof(std::uint32_t) * startIndice;
		else
			offsetIndices = offsetIndices + _indexOffset + sizeof(std::uint16_t) * startIndice;

		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawElementsInstancedBaseVertex(drawType, numIndices, _indexType, offsetIndices, numInstances, startVertice);
	}
}

void
OGLDeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_glcontext->getActive());

	if (target)
	{
		auto framebuffer = target->downcast_pointer<OGLFramebuffer>();
		if (_framebuffer != framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getInstanceID());

			auto& framebufferDesc = framebuffer->getGraphicsFramebufferDesc();
			this->setViewport(0, Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));

			_framebuffer = framebuffer;
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		_framebuffer = nullptr;
	}
}

void
OGLDeviceContext::setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
}

void
OGLDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext->getActive());

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
	{
		auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		glClearBufferfv(GL_DEPTH, 0, &f);

		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, 0, &s);
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
	{
		glClearBufferfv(GL_COLOR, i, color.ptr());
	}
}

void
OGLDeviceContext::discardFramebuffer(GraphicsAttachmentType attachments[], std::size_t numAttachment) noexcept
{
	assert(_glcontext->getActive());
	_framebuffer->discard(attachments, numAttachment);
}

void
OGLDeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);
	assert(src->isInstanceOf<OGLFramebuffer>());
	assert(!dest || (dest && dest->isInstanceOf<OGLFramebuffer>()));
	assert(_glcontext->getActive());

	auto readFramebuffer = src->downcast<OGLFramebuffer>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast<OGLFramebuffer>()->getInstanceID() : GL_NONE;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

GraphicsFramebufferPtr
OGLDeviceContext::getFramebuffer() const noexcept
{
	if (_framebuffer)
		return _framebuffer->upcast_pointer<GraphicsFramebuffer>();
	return nullptr;
}

void
OGLDeviceContext::present() noexcept
{
	assert(_glcontext->getActive());
	_glcontext->present();
}

bool
OGLDeviceContext::isTextureSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportTextures.begin(), _supportTextures.end(), format) != _supportTextures.end();
}

bool
OGLDeviceContext::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return std::find(_supportTextureDims.begin(), _supportTextureDims.end(), dimension) != _supportTextureDims.end();
}

bool
OGLDeviceContext::isVertexSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportAttribute.begin(), _supportAttribute.end(), format) != _supportAttribute.end();
}

bool
OGLDeviceContext::isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept
{
	return std::find(_supportShaders.begin(), _supportShaders.end(), stage) != _supportShaders.end();
}

bool
OGLDeviceContext::checkSupport() noexcept
{
	if (!GLEW_EXT_texture_object)
	{
		GL_PLATFORM_LOG("Can't support GL_EXT_texture_object.");
		return false;
	}

	if (!GLEW_EXT_packed_pixels)
	{
		GL_PLATFORM_LOG("Can't support GL_EXT_packed_pixels.");
		return false;
	}

	if (!GLEW_ARB_draw_elements_base_vertex)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_draw_elements_base_vertex.");
		return false;
	}

	if (!GLEW_ARB_uniform_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_uniform_buffer_object.");
		return false;
	}

	if (!GLEW_ARB_texture_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_texture_buffer_object.");
		return false;
	}

	if (!GLEW_ARB_sampler_objects)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_sampler_objects.");
		return false;
	}

	if (!GLEW_ARB_framebuffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_framebuffer_object.");
		return false;
	}

	if (!GLEW_EXT_texture_filter_anisotropic)
	{
		GL_PLATFORM_LOG("Can't support GL_EXT_texture_filter_anisotropic.");
		return false;
	}

	if (!GLEW_ARB_separate_shader_objects)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_separate_shader_objects.");
		return false;
	}

	return true;
}

bool
OGLDeviceContext::initTextureSupports() noexcept
{
	if (GLEW_VERSION_2_0 || GLEW_EXT_texture)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	}

	if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_bgra)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
	}

	if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_abgr)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32);
	}

	if (GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	}

	if (GLEW_EXT_abgr && GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32);
	}

	if (GLEW_ARB_texture_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (GLEW_EXT_packed_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	}

	if (GLEW_EXT_texture_shared_exponent)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (GLEW_ARB_texture_stencil8)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	}

	if (GLEW_ARB_depth_texture)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (GLEW_ARB_depth_buffer_float || GLEW_NV_depth_buffer_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt);
	}

	if (GLEW_EXT_packed_depth_stencil || GLEW_ARB_framebuffer_object)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}

	if (GLEW_ARB_texture_rgb10_a2ui)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	}

	if (GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (GLEW_EXT_abgr && GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (GLEW_ARB_texture_rg)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	}

	if (GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32);
	}

	if (GLEW_EXT_texture_compression_s3tc)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC5UNormBlock);
	}

	if (GLEW_ARB_ES2_compatibility)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
	}

	if (GLEW_ARB_ES3_compatibility)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11SNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11SNormBlock);
	}

	if (GLEW_ARB_ES3_compatibility && GL_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock);
	}

	if (GLEW_KHR_texture_compression_astc_ldr || GL_KHR_texture_compression_astc_hdr)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
	}

	if ((GLEW_KHR_texture_compression_astc_ldr || GL_KHR_texture_compression_astc_hdr) && GL_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);
	}

	return true;
}

bool
OGLDeviceContext::initTextureDimSupports() noexcept
{
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);

	if (GLEW_ARB_texture_cube_map_array)
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	return true;
}

bool
OGLDeviceContext::initVertexSupports() noexcept
{
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);

	if (GLEW_ARB_vertex_attrib_64bit || GLEW_EXT_vertex_attrib_64bit)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64UInt);
	}

	if (GLEW_ARB_vertex_type_10f_11f_11f_rev)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	}

	if (GLEW_ARB_vertex_type_2_10_10_10_rev)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);

		if (GLEW_ARB_vertex_array_bgra)
		{
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32);
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
		}
	}

	if (GLEW_ARB_vertex_array_bgra)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	return true;
}

bool
OGLDeviceContext::initShaderSupports() noexcept
{
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);

	if (GLEW_ARB_geometry_shader4)
		_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit);
	return true;
}

bool
OGLDeviceContext::initDebugControl(const GraphicsContextDesc& desc) noexcept
{
	if (!desc.getDebugMode())
		return true;

	if (GLEW_ARB_debug_output)
	{
		// 131184 memory info
		// 131185 memory allocation info
		GLuint ids[] =
		{
			131076,
			131169,
			131184,
			131185,
			131218,
			131204
		};

		glEnable(GL_DEBUG_OUTPUT);

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(debugCallBack, this);
		// enable all
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
		// disable ids
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);

		return true;
	}

	return false;
}

bool
OGLDeviceContext::initStateSystem() noexcept
{
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearStencil(0);

	glViewport(0, 0, 0, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

	glDisable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &_globalVao);
	glBindVertexArray(_globalVao);

	return true;
}

void
OGLDeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
	std::cerr << "source : ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		std::cerr << "GL_DEBUG_SOURCE_API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cerr << "GL_DEBUG_SOURCE_OTHER";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "type : ";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << "GL_DEBUG_TYPE_ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cerr << "GL_DEBUG_TYPE_OTHER";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cerr << "GL_DEBUG_TYPE_MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "id : " << id << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cerr << "notice";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "low";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "medium";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cerr << "high";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "message : " << message << std::endl;
}

void
OGLDeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLDeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END