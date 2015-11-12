// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/render_mesh.h>
#include <ray/render_scene.h>
#include <ray/render_buffer.h>

_NAME_BEGIN

RenderMesh::RenderMesh() noexcept
{
}

RenderMesh::~RenderMesh() noexcept
{
}

void
RenderMesh::setMaterial(MaterialPtr material) noexcept
{
	_material = material;
}

MaterialPtr
RenderMesh::getMaterial() noexcept
{
	return _material;
}

void
RenderMesh::setRenderBuffer(RenderBufferPtr geometry) noexcept
{
	assert(geometry);

	_geometry = geometry;

	if (!_renderable)
	{
		_renderable = std::make_shared<RenderIndirect>();
		_renderable->startVertice = 0;
		_renderable->startIndice = 0;
		_renderable->numVertices = geometry->getNumVertices();
		_renderable->numIndices = geometry->getNumIndices();
		_renderable->numInstances = 0;
	}
}

RenderBufferPtr
RenderMesh::getRenderBuffer() noexcept
{
	return _geometry;
}

void 
RenderMesh::setRenderIndirect(RenderIndirectPtr renderable) noexcept
{
	_renderable = renderable;
}

RenderIndirectPtr
RenderMesh::getRenderIndirect() noexcept
{
	return _renderable;
}

void
RenderMesh::setRenderScene(RenderScenePtr scene) noexcept
{
	if (_renderScene.lock() != scene)
	{
		auto renderScene = _renderScene.lock();
		if (renderScene)
		{
			renderScene->removeRenderObject(this->shared_from_this());
		}

		_renderScene = scene;

		if (scene)
		{
			scene->addRenderObject(this->shared_from_this());
		}
	}
}

RenderScenePtr
RenderMesh::getRenderScene() const noexcept
{
	return _renderScene.lock();
}

_NAME_END