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
#include "vk_sampler.h"
#include "vk_device.h"

_NAME_BEGIN

__ImplementSubClass(VulkanSampler, GraphicsSampler, "VulkanSampler")

VulkanSampler::VulkanSampler() noexcept
	: _vkSampler(VK_NULL_HANDLE)
{
}

VulkanSampler::~VulkanSampler() noexcept
{
}

bool
VulkanSampler::setup(const GraphicsSamplerDesc& samplerDesc) noexcept
{
	_samplerDesc = samplerDesc;
	return true;
}

void
VulkanSampler::close() noexcept
{
	if (_vkSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkSampler, nullptr);
		_vkSampler = VK_NULL_HANDLE;
	}
}

VkSampler
VulkanSampler::getSampler() const noexcept
{
	return _vkSampler;
}

const GraphicsSamplerDesc&
VulkanSampler::getGraphicsSamplerDesc() const noexcept
{
	return _samplerDesc;
}

void
VulkanSampler::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanSampler::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END