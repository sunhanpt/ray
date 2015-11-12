// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include <ray/material_semantic.h>

_NAME_BEGIN

MaterialSemantic::MaterialSemantic() noexcept
{
	this->setup();
}

MaterialSemantic::~MaterialSemantic() noexcept
{
	this->close();
}

void
MaterialSemantic::setup() noexcept
{
	_semantics.resize(NumSemantic);

	_semantics[matModel] = std::make_shared<MaterialParam>("matModel", float4x4());
	_semantics[matModelInverse] = std::make_shared<MaterialParam>("matModelInverse", float4x4());
	_semantics[matModelInverseTranspose] = std::make_shared<MaterialParam>("matModelInverseTranspose", float4x4());
	_semantics[matProject] = std::make_shared<MaterialParam>("matProject", float4x4());
	_semantics[matProjectInverse] = std::make_shared<MaterialParam>("matProjectInverse", float4x4());
	_semantics[matView] = std::make_shared<MaterialParam>("matView", float4x4());
	_semantics[matViewInverse] = std::make_shared<MaterialParam>("matViewInverse", float4x4());
	_semantics[matViewInverseTranspose] = std::make_shared<MaterialParam>("matViewInverseTranspose", float4x4());
	_semantics[matViewProject] = std::make_shared<MaterialParam>("matViewProject", float4x4());
	_semantics[matViewProjectInverse] = std::make_shared<MaterialParam>("matViewProjectInverse", float4x4());

	_semantics[CameraAperture] = std::make_shared<MaterialParam>("CameraAperture", float());
	_semantics[CameraFar] = std::make_shared<MaterialParam>("CameraFar", float());
	_semantics[CameraNear] = std::make_shared<MaterialParam>("CameraNear", float());
	_semantics[CameraView] = std::make_shared<MaterialParam>("CameraView", float3());
	_semantics[CameraPosition] = std::make_shared<MaterialParam>("CameraPosition", Vector3());
	_semantics[CameraDirection] = std::make_shared<MaterialParam>("CameraDirection", Vector3());

	_semantics[DepthMap] = std::make_shared<MaterialParam>("DepthMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[ColorMap] = std::make_shared<MaterialParam>("ColorMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[NormalMap] = std::make_shared<MaterialParam>("NormalMap", ShaderVariantType::SPT_TEXTURE);

	_semantics[DeferredDepthMap] = std::make_shared<MaterialParam>("DeferredDepthMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[DeferredDepthLinearMap] = std::make_shared<MaterialParam>("DeferredDepthLinearMap", ShaderVariantType::SPT_TEXTURE);	
	_semantics[DeferredGraphicMap] = std::make_shared<MaterialParam>("DeferredGraphicMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[DeferredNormalMap] = std::make_shared<MaterialParam>("DeferredNormalMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[DeferredLightMap] = std::make_shared<MaterialParam>("DeferredLightMap", ShaderVariantType::SPT_TEXTURE);
	_semantics[DeferredShadowMap] = std::make_shared<MaterialParam>("DeferredShadowMap", ShaderVariantType::SPT_TEXTURE);

	for (auto& it : _semantics)
	{
		it->setSemantic(it->getName());
	}
}

void
MaterialSemantic::close() noexcept
{
	_semantics.clear();
}

void
MaterialSemantic::setMatrixParam(GlobalMatrixSemantic index, const Matrix4x4& m) noexcept
{
	_semantics[index]->assign(m);
}

const Matrix4x4&
MaterialSemantic::getMatrixParam(GlobalMatrixSemantic index) noexcept
{
	return _semantics[index]->getFloat4x4();
}

std::string
MaterialSemantic::getMatrixParmName(GlobalMatrixSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

GlobalMatrixSemantic
MaterialSemantic::getMatrixParamSemantic(const std::string& name) const noexcept
{
	if (getMatrixParmName(GlobalMatrixSemantic::matModel) == name)
		return GlobalMatrixSemantic::matModel;
	if (getMatrixParmName(GlobalMatrixSemantic::matModelInverse) == name)
		return  GlobalMatrixSemantic::matModelInverse;
	if (getMatrixParmName(GlobalMatrixSemantic::matModelInverseTranspose) == name)
		return GlobalMatrixSemantic::matModelInverseTranspose;
	if (getMatrixParmName(GlobalMatrixSemantic::matView) == name)
		return GlobalMatrixSemantic::matView;
	if (getMatrixParmName(GlobalMatrixSemantic::matViewInverse) == name)
		return GlobalMatrixSemantic::matViewInverse;
	if (getMatrixParmName(GlobalMatrixSemantic::matViewInverseTranspose) == name)
		return GlobalMatrixSemantic::matViewInverseTranspose;
	if (getMatrixParmName(GlobalMatrixSemantic::matProject) == name)
		return GlobalMatrixSemantic::matProject;
	if (getMatrixParmName(GlobalMatrixSemantic::matProjectInverse) == name)
		return GlobalMatrixSemantic::matProjectInverse;
	if (getMatrixParmName(GlobalMatrixSemantic::matViewProject) == name)
		return GlobalMatrixSemantic::matViewProject;
	if (getMatrixParmName(GlobalMatrixSemantic::matViewProjectInverse) == name)
		return GlobalMatrixSemantic::matViewProjectInverse;

	return NotSemantic;
}

void
MaterialSemantic::setFloatParam(GlobalFloatSemantic index, float v) noexcept
{
	_semantics[index]->assign(v);
}

float
MaterialSemantic::getFloatParam(GlobalFloatSemantic index) noexcept
{
	return _semantics[index]->getFloat();
}

std::string
MaterialSemantic::getFloatParmName(GlobalFloatSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

GlobalFloatSemantic
MaterialSemantic::getFloatParamSemantic(const std::string& name) const noexcept
{
	if (getFloatParmName(GlobalFloatSemantic::CameraAperture) == name)
		return GlobalFloatSemantic::CameraAperture;
	if (getFloatParmName(GlobalFloatSemantic::CameraFar) == name)
		return GlobalFloatSemantic::CameraFar;
	if (getFloatParmName(GlobalFloatSemantic::CameraNear) == name)
		return GlobalFloatSemantic::CameraNear;

	return (GlobalFloatSemantic)NotSemantic;
}

void
MaterialSemantic::setFloat3Param(GlobalFloat3Semantic index, const float3& v) noexcept
{
	_semantics[index]->assign(v);
}

const Vector3&
MaterialSemantic::getFloat3Param(GlobalFloat3Semantic index) noexcept
{
	return _semantics[index]->getFloat3();
}

std::string
MaterialSemantic::getFloat3ParmName(GlobalFloat3Semantic index) const noexcept
{
	return _semantics[index]->getName();
}

GlobalFloat3Semantic
MaterialSemantic::getFloat3ParamSemantic(const std::string& name) const noexcept
{
	if (getFloat3ParmName(GlobalFloat3Semantic::CameraView) == name)
		return GlobalFloat3Semantic::CameraView;
	if (getFloat3ParmName(GlobalFloat3Semantic::CameraPosition) == name)
		return GlobalFloat3Semantic::CameraPosition;
	if (getFloat3ParmName(GlobalFloat3Semantic::CameraDirection) == name)
		return GlobalFloat3Semantic::CameraDirection;
	return (GlobalFloat3Semantic)NotSemantic;
}

void
MaterialSemantic::setFloat4Param(GlobalFloat4Semantic index, const float4& v) noexcept
{
	_semantics[index]->assign(v);
}

const Vector4&
MaterialSemantic::getFloat4Param(GlobalFloat4Semantic index) noexcept
{
	return _semantics[index]->getFloat4();
}

std::string
MaterialSemantic::getFloat4ParmName(GlobalFloat4Semantic index) const noexcept
{
	return _semantics[index]->getName();
}

GlobalFloat4Semantic
MaterialSemantic::getFloat4ParamSemantic(const std::string& name) const noexcept
{
	return (GlobalFloat4Semantic)NotSemantic;
}

void
MaterialSemantic::setTexParam(GlobalTexSemantic index, TexturePtr texture) noexcept
{
	_semantics[index]->assign(texture);
}

TexturePtr
MaterialSemantic::getTexParam(GlobalTexSemantic index) noexcept
{
	return _semantics[index]->getTexture();
}

std::string
MaterialSemantic::getTexParmName(GlobalTexSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

GlobalTexSemantic
MaterialSemantic::getTexParamSemantic(const std::string& name) const noexcept
{
	if (getTexParmName(GlobalTexSemantic::DepthMap) == name)
		return GlobalTexSemantic::DepthMap;
	if (getTexParmName(GlobalTexSemantic::ColorMap) == name)
		return GlobalTexSemantic::ColorMap;
	if (getTexParmName(GlobalTexSemantic::DeferredDepthMap) == name)
		return GlobalTexSemantic::DeferredDepthMap;
	if (getTexParmName(GlobalTexSemantic::DeferredGraphicMap) == name)
		return GlobalTexSemantic::DeferredGraphicMap;
	if (getTexParmName(GlobalTexSemantic::DeferredNormalMap) == name)
		return GlobalTexSemantic::DeferredNormalMap;
	if (getTexParmName(GlobalTexSemantic::DeferredLightMap) == name)
		return GlobalTexSemantic::DeferredLightMap;
	return (GlobalTexSemantic)NotSemantic;
}

MaterialParamPtr
MaterialSemantic::getParamPointer(const std::string& name) const noexcept
{
	for (auto& it : _semantics)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	return nullptr;
}

_NAME_END