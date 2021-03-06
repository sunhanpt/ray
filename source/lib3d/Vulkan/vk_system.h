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
#ifndef _H_VK_SYSTEM_H_
#define _H_VK_SYSTEM_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanSystem final
{
	__DeclareSingleton(VulkanSystem)
public:
	VulkanSystem() noexcept;
	~VulkanSystem() noexcept;

	bool open() noexcept;
	void close() noexcept;

	VkInstance getInstance() const noexcept;

	void getInstanceLayerNames(std::vector<char*>& instanceLayerNames) noexcept;
	void getInstanceExtensitionNames(std::vector<char*>& instanceLayerNames) noexcept;

	const GraphicsDevicePropertys& getPhysicalDevices() const noexcept;

	void startDebugControl() noexcept;
	void stopDebugControl() noexcept;

	void print(const char* message, ...) noexcept;

private:
	bool initInstance() noexcept;
	bool initPhysicalDevices() noexcept;

	bool checkInstanceLayer(std::size_t instanceEnabledLayerCount, const char* instanceValidationLayerNames[]) noexcept;
	bool checkInstanceExtenstion(std::size_t instanceEnabledExtensition, const char* instanceValidationExtensitionName[]) noexcept;

private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL dbgFunc(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserData);

private:
	VulkanSystem(const VulkanSystem&) noexcept = delete;
	VulkanSystem& operator=(const VulkanSystem&) noexcept = delete;

private:
	bool _isOpened;

	VkInstance _instance;
	VkDebugReportCallbackEXT _debugHandle;

	std::vector<VkLayerProperties> _instanceLayers;
	std::vector<VkExtensionProperties> _instanceExtensions;
	GraphicsDevicePropertys _physicalDevices;
};

_NAME_END

#endif