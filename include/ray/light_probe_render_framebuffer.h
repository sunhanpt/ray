// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_LIGHTPROBE_RENDER_FRAMEBUFFER_H_
#define _H_LIGHTPROBE_RENDER_FRAMEBUFFER_H_

#include <ray/render_pipeline_framebuffer.h>

_NAME_BEGIN

class EXPORT LightProbeRenderFramebuffer : public RenderPipelineFramebuffer
{
	__DeclareSubInterface(ShadowRenderFramebuffer, RenderPipelineFramebuffer)
public:
	LightProbeRenderFramebuffer() noexcept;
	virtual ~LightProbeRenderFramebuffer() noexcept;

	bool setup();

	const GraphicsTexturePtr& getDepthMap() const noexcept { return _probeDepthMap; }
	const GraphicsTexturePtr& getColorMap() const noexcept { return _probeColorMap; }
	const GraphicsTexturePtr& getNormalMap() const noexcept { return _probeNormalMap; }

	const GraphicsFramebufferPtr& getRSMView() const noexcept { return _probeRSMView; }

private:
	LightProbeRenderFramebuffer(const LightProbeRenderFramebuffer&) noexcept = delete;
	LightProbeRenderFramebuffer& operator=(const LightProbeRenderFramebuffer&) noexcept = delete;

private:
	GraphicsTexturePtr _probeDepthMap;
	GraphicsTexturePtr _probeColorMap;
	GraphicsTexturePtr _probeNormalMap;

	GraphicsFramebufferPtr _probeRSMView;
	GraphicsFramebufferLayoutPtr _probeRSMViewLayout;
};

_NAME_END

#endif