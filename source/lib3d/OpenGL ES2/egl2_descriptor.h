// +----------------------------------------------------------------------
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
#ifndef _H_EGL2_DESCRIPTOR_H_
#define _H_EGL2_DESCRIPTOR_H_

#include "egl2_types.h"

_NAME_BEGIN

class EGL2GraphicsUniformSet final : public GraphicsUniformSet
{
	__DeclareSubClass(EGL2GraphicsUniformSet, GraphicsUniformSet)
public:
	EGL2GraphicsUniformSet() noexcept;
	virtual ~EGL2GraphicsUniformSet() noexcept;

	void uniform1b(bool value) noexcept;
	void uniform1i(std::int32_t i1) noexcept;
	void uniform2i(const int2& value) noexcept;
	void uniform2i(std::int32_t i1, std::int32_t i2) noexcept;
	void uniform3i(const int3& value) noexcept;
	void uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept;
	void uniform4i(const int4& value) noexcept;
	void uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept;
	void uniform1ui(std::uint32_t i1) noexcept;
	void uniform2ui(const uint2& value) noexcept;
	void uniform2ui(std::uint32_t i1, std::uint32_t i2) noexcept;
	void uniform3ui(const uint3& value) noexcept;
	void uniform3ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3) noexcept;
	void uniform4ui(const uint4& value) noexcept;
	void uniform4ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) noexcept;
	void uniform1f(float i1) noexcept;
	void uniform2f(const float2& value) noexcept;
	void uniform2f(float i1, float i2) noexcept;
	void uniform3f(const float3& value) noexcept;
	void uniform3f(float i1, float i2, float i3) noexcept;
	void uniform4f(const float4& value) noexcept;
	void uniform4f(float i1, float i2, float i3, float i4) noexcept;
	void uniform2fmat(const float* mat2) noexcept;
	void uniform2fmat(const float2x2& value) noexcept;
	void uniform3fmat(const float* mat3) noexcept;
	void uniform3fmat(const float3x3& value) noexcept;
	void uniform4fmat(const float* mat4) noexcept;
	void uniform4fmat(const float4x4& value) noexcept;
	void uniform1iv(const std::vector<int1>& value) noexcept;
	void uniform1iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform2iv(const std::vector<int2>& value) noexcept;
	void uniform2iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform3iv(const std::vector<int3>& value) noexcept;
	void uniform3iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform4iv(const std::vector<int4>& value) noexcept;
	void uniform4iv(std::size_t num, const std::int32_t* str) noexcept;
	void uniform1uiv(const std::vector<uint1>& value) noexcept;
	void uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform2uiv(const std::vector<uint2>& value) noexcept;
	void uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform3uiv(const std::vector<uint3>& value) noexcept;
	void uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform4uiv(const std::vector<uint4>& value) noexcept;
	void uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept;
	void uniform1fv(const std::vector<float1>& value) noexcept;
	void uniform1fv(std::size_t num, const float* str) noexcept;
	void uniform2fv(const std::vector<float2>& value) noexcept;
	void uniform2fv(std::size_t num, const float* str) noexcept;
	void uniform3fv(const std::vector<float3>& value) noexcept;
	void uniform3fv(std::size_t num, const float* str) noexcept;
	void uniform4fv(const std::vector<float4>& value) noexcept;
	void uniform4fv(std::size_t num, const float* str) noexcept;
	void uniform2fmatv(const std::vector<float2x2>& value) noexcept;
	void uniform2fmatv(std::size_t num, const float* mat2) noexcept;
	void uniform3fmatv(const std::vector<float3x3>& value) noexcept;
	void uniform3fmatv(std::size_t num, const float* mat3) noexcept;
	void uniform4fmatv(const std::vector<float4x4>& value) noexcept;
	void uniform4fmatv(std::size_t num, const float* mat4) noexcept;
	void uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept;
	void uniformBuffer(GraphicsDataPtr ubo) noexcept;

	bool getBool() const noexcept;
	int getInt() const noexcept;
	const int2& getInt2() const noexcept;
	const int3& getInt3() const noexcept;
	const int4& getInt4() const noexcept;
	uint getUInt() const noexcept;
	const uint2& getUInt2() const noexcept;
	const uint3& getUInt3() const noexcept;
	const uint4& getUInt4() const noexcept;
	float getFloat() const noexcept;
	const float2& getFloat2() const noexcept;
	const float3& getFloat3() const noexcept;
	const float4& getFloat4() const noexcept;
	const float2x2& getFloat2x2() const noexcept;
	const float3x3& getFloat3x3() const noexcept;
	const float4x4& getFloat4x4() const noexcept;
	const std::vector<int1>& getIntArray() const noexcept;
	const std::vector<int2>& getInt2Array() const noexcept;
	const std::vector<int3>& getInt3Array() const noexcept;
	const std::vector<int4>& getInt4Array() const noexcept;
	const std::vector<uint1>& getUIntArray() const noexcept;
	const std::vector<uint2>& getUInt2Array() const noexcept;
	const std::vector<uint3>& getUInt3Array() const noexcept;
	const std::vector<uint4>& getUInt4Array() const noexcept;
	const std::vector<float1>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;
	const std::vector<float3>& getFloat3Array() const noexcept;
	const std::vector<float4>& getFloat4Array() const noexcept;
	const std::vector<float2x2>& getFloat2x2Array() const noexcept;
	const std::vector<float3x3>& getFloat3x3Array() const noexcept;
	const std::vector<float4x4>& getFloat4x4Array() const noexcept;
	const GraphicsTexturePtr& getTexture() const noexcept;
	const GraphicsSamplerPtr& getTextureSampler() const noexcept;
	const GraphicsDataPtr& getBuffer() const noexcept;

	void setGraphicsParam(GraphicsParamPtr param) noexcept;
	const GraphicsParamPtr& getGraphicsParam() const noexcept;

private:
	EGL2GraphicsUniformSet(const EGL2GraphicsUniformSet&) = delete;
	EGL2GraphicsUniformSet& operator=(const EGL2GraphicsUniformSet&) = delete;

private:
	GraphicsParamPtr _param;
	GraphicsVariant _variant;
};

class EGL2DescriptorPool final : public GraphicsDescriptorPool
{
	__DeclareSubClass(EGL2DescriptorPool, GraphicsDescriptorPool)
public:
	EGL2DescriptorPool() noexcept;
	~EGL2DescriptorPool() noexcept;

	bool setup(const GraphicsDescriptorPoolDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsDescriptorPoolDesc& getGraphicsDescriptorPoolDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorPool(const EGL2DescriptorPool&) noexcept = delete;
	EGL2DescriptorPool& operator=(const EGL2DescriptorPool&) noexcept = delete;

private:
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorPoolDesc _descriptorPoolDesc;
};

class EGL2DescriptorSetLayout final : public GraphicsDescriptorSetLayout
{
	__DeclareSubClass(EGL2DescriptorSetLayout, OGLCoreGraphicsData)
public:
	EGL2DescriptorSetLayout() noexcept;
	~EGL2DescriptorSetLayout() noexcept;

	bool setup(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsDescriptorSetLayoutDesc& getGraphicsDescriptorSetLayoutDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorSetLayout(const EGL2DescriptorSetLayout&) noexcept = delete;
	EGL2DescriptorSetLayout& operator=(const EGL2DescriptorSetLayout&) noexcept = delete;

private:
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorSetLayoutDesc _descriptorSetDesc;
};

class EGL2DescriptorSet final : public GraphicsDescriptorSet
{
	__DeclareSubClass(EGL2DescriptorSet, OGLCoreGraphicsData)
public:
	EGL2DescriptorSet() noexcept;
	~EGL2DescriptorSet() noexcept;

	bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
	void close() noexcept;

	void apply(const EGL2Program& program) noexcept;

	void copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

	const GraphicsUniformSets& getGraphicsUniformSets() const noexcept;
	const GraphicsDescriptorSetDesc& getGraphicsDescriptorSetDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorSet(const EGL2DescriptorSet&) noexcept = delete;
	EGL2DescriptorSet& operator=(const EGL2DescriptorSet&) noexcept = delete;

private:
	GraphicsUniformSets _activeUniformSets;
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorSetDesc _descriptorSetDesc;
};

_NAME_END

#endif