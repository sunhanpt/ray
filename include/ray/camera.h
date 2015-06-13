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
#ifndef _H_CAMERA_H_
#define _H_CAMERA_H_

#include <ray/render_object.h>

_NAME_BEGIN

enum CameraType
{
    CT_ORTHO,
    CT_PERSPECTIVE
};

enum CameraOrder
{
    CO_SHADOW,
    CO_DEPTH_OPAQUES,
    CO_DEPTH_TRANSPARENT,
    CO_COLOR,
    CO_NORMAL,
    CO_LIGHT,
    CO_SHADING,
    CO_CUSTOM,
    CO_MAIN
};

enum CameraRender
{
    CR_RENDER_TO_TEXTURE,
    CR_RENDER_TO_CUBEMAP,
};

class EXPORT Camera final : public RenderObject
{
public:
    Camera() noexcept;
    ~Camera() noexcept;

    void setup(std::size_t w, std::size_t h) noexcept;
    void close() noexcept;

    float getAperture() const noexcept;
    float getRatio() const noexcept;
    float getNear() const noexcept;
    float getFar() const noexcept;

    const Vector3& getTranslate() const noexcept;
    const Vector3& getLookAt() const noexcept;
    const Vector3& getUpVector() const noexcept;

    const Matrix4x4& getView() const noexcept;
    const Matrix4x4& getViewInverse() const noexcept;
    const Matrix4x4& getViewInverseTranspose() const noexcept;

    const Matrix4x4& getProject() const noexcept;
    const Matrix4x4& getProjectInverse() const noexcept;

    const Matrix4x4& getViewProject() const noexcept;
    const Matrix4x4& getViewProjectInverse() const noexcept;

    const Vector2& getProjLength() const noexcept;
    const Vector4& getProjConstant() const noexcept;

    const Vector4& getClipConstant() const noexcept;

    void makeLookAt(const Vector3& pos, const Vector3& lookat, const Vector3& up) noexcept;
    void makeOrtho(float left, float right, float bottom, float top, float znear, float zfar) noexcept;
    void makePerspective(float aperture, float ratio, float znear, float zfar) noexcept;
    void makeViewProject() noexcept;

    Vector3 project(const Vector3& pos) const noexcept;
    Vector3 unproject(const Vector3& pos) const noexcept;

    void setViewport(const Viewport& viewport) noexcept;
    const Viewport&  getViewport() const noexcept;

    void setCameraType(CameraType type) noexcept;
    void setCameraOrder(CameraOrder order) noexcept;
    void setCameraRender(CameraRender mode) noexcept;

    void setRenderScene(RenderScene* scene) noexcept;
    void setRenderWindow(RenderWindowPtr vp) noexcept;
    void setRenderTexture(RenderTexturePtr texture) noexcept;

    CameraType getCameraType() const noexcept;
    CameraOrder getCameraOrder() const noexcept;
    CameraRender getCameraRender() const noexcept;

    RenderScene* getRenderScene() const noexcept;
    RenderWindowPtr getRenderWindow() const noexcept;
    RenderTexturePtr getRenderTexture() const noexcept;
    RenderTexturePtr getSwapTexture() const noexcept;

    RenderTexturePtr getDeferredDepthMap() const noexcept;
    RenderTexturePtr getDeferredGraphicMap() const noexcept;
    RenderTexturePtr getDeferredNormalMap() const noexcept;
    RenderTexturePtr getDeferredLightMap() const noexcept;

    MultiRenderTexturePtr getDeferredGraphicsMaps() const noexcept;

    CameraPtr clone() const noexcept;

private:
    Camera(const Camera&) noexcept = delete;
    Camera& operator=(const Camera&) noexcept = delete;

private:

    float _left;
    float _right;
    float _top;
    float _bottom;
    float _aperture;
    float _ratio;
    float _zNear;
    float _zFar;

    Vector3 _translate;
    Vector3 _lookat;
    Vector3 _up;

    Vector2 _projLength;
    Vector4 _projConstant;
    Vector4 _clipConstant;

    Matrix4x4 _view;
    Matrix4x4 _viewInverse;
    Matrix4x4 _viewInverseTranspose;

    Matrix4x4 _project;
    Matrix4x4 _projectInverse;

    Matrix4x4 _viewProejct;
    Matrix4x4 _viewProjectInverse;

    Viewport _viewport;

    CameraType      _cameraType;
    CameraOrder     _cameraOrder;
    CameraRender    _cameraRender;

    RenderTexturePtr _deferredDepthMap;
    RenderTexturePtr _deferredGraphicMap;
    RenderTexturePtr _deferredNormalMap;
    RenderTexturePtr _deferredLightMap;

    MultiRenderTexturePtr _deferredGraphicMaps;

    RenderTexturePtr _swapTexture;
    RenderTexturePtr _renderTexture;
    RenderTexturePtr _cubeTexture;

    RenderScene*  _renderScene;
    RenderWindowPtr _renderWindow;
};

_NAME_END

#endif