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
#include <ray/render_pipeline.h>
#include <ray/render_buffer.h>
#include <ray/post_process.h>
#include <ray/model.h>
#include <ray/render_scene.h>

_NAME_BEGIN

void
RenderDataManager::addRenderData(RenderQueue queue, RenderObject* object) noexcept
{
    _visiable[queue].push_back(object);
}

RenderData&
RenderDataManager::getRenderData(RenderQueue queue) noexcept
{
    return _visiable[queue];
}

void
RenderDataManager::clear() noexcept
{
    _visiable[RenderQueue::Background].clear();
    _visiable[RenderQueue::Shadow].clear();
    _visiable[RenderQueue::Opaque].clear();
    _visiable[RenderQueue::Transparent].clear();
    _visiable[RenderQueue::DeferredLight].clear();
}

RenderPipeline::RenderPipeline() noexcept
{
}

RenderPipeline::~RenderPipeline() noexcept
{
    this->close();
}

void
RenderPipeline::setup(std::size_t width, std::size_t height) except
{
    MeshProperty mesh;
    mesh.makePlane(2, 2, 1, 1);

    _renderSceneQuad = std::make_shared<RenderBuffer>();
    _renderSceneQuad->setup(mesh);

    mesh.makeSphere(1, 16, 12);

    _renderSphere = std::make_shared<RenderBuffer>();
    _renderSphere->setup(mesh);

    mesh.makeCone(1, 1, 16);

    _renderCone = std::make_shared<RenderBuffer>();
    _renderCone->setup(mesh);
}

void
RenderPipeline::close() noexcept
{
    if (_renderSceneQuad)
    {
        _renderSceneQuad.reset();
        _renderSceneQuad = nullptr;
    }

    if (_renderSphere)
    {
        _renderSphere.reset();
        _renderSphere = nullptr;
    }

    if (_renderCone)
    {
        _renderCone.reset();
        _renderCone = nullptr;
    }

    for (auto& it : _postprocessors)
    {
        it->setActive(false);
    }
}

void
RenderPipeline::setCamera(Camera* camera) noexcept
{
    _camera = camera;
}

Camera*
RenderPipeline::getCamera() const noexcept
{
    return _camera;
}

void
RenderPipeline::setRenderer(RendererPtr renderer) noexcept
{
    _renderer = renderer;
}

RendererPtr
RenderPipeline::getRenderer() const noexcept
{
    return _renderer;
}

void
RenderPipeline::addRenderData(RenderQueue queue, RenderObject* object) noexcept
{
    _renderDataManager.addRenderData(queue, object);
}

RenderData&
RenderPipeline::getRenderData(RenderQueue queue) noexcept
{
    return _renderDataManager.getRenderData(queue);
}

void
RenderPipeline::drawSceneQuad() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderSceneQuad->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderSceneQuad->getNumIndices();
    renderable.numInstances = 0;
    _renderer->drawMesh(_renderSceneQuad, renderable);
}

void
RenderPipeline::drawSphere() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderSphere->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderSphere->getNumIndices();
    renderable.numInstances = 0;
    _renderer->drawMesh(_renderSphere, renderable);
}

void
RenderPipeline::drawCone() noexcept
{
    Renderable renderable;
    renderable.type = VertexType::GPU_TRIANGLE;
    renderable.startVertice = 0;
    renderable.numVertices = _renderCone->getNumVertices();
    renderable.startIndice = 0;
    renderable.numIndices = _renderCone->getNumIndices();
    renderable.numInstances = 0;
    _renderer->drawMesh(_renderCone, renderable);
}

void
RenderPipeline::drawRenderable(RenderQueue queue, RenderPass pass, MaterialPassPtr material) noexcept
{
    auto& renderable = _renderDataManager.getRenderData(queue);
    for (auto& it : renderable)
    {
        it->render(_renderer, queue, pass, material);
    }
}

void
RenderPipeline::setRenderTexture(RenderTexturePtr texture) noexcept
{
    _renderer->setRenderTexture(texture);
}

void
RenderPipeline::setRenderTexture(MultiRenderTexturePtr texture) noexcept
{
    _renderer->setRenderTexture(texture);
}

void
RenderPipeline::setTechnique(MaterialPassPtr pass) noexcept
{
    _renderer->setRenderState(pass->getRenderState());
    _renderer->setShaderObject(pass->getShaderObject());
}

void
RenderPipeline::readRenderTexture(RenderTexturePtr texture, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
    _renderer->readRenderTexture(texture, pfd, w, h, data);
}

void
RenderPipeline::copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept
{
    _renderer->copyRenderTexture(srcTarget, src, destTarget, dest);
}

void
RenderPipeline::addPostProcess(RenderPostProcessPtr postprocess) noexcept
{
    auto it = std::find(_postprocessors.begin(), _postprocessors.end(), postprocess);
    if (it == _postprocessors.end())
    {
        postprocess->setActive(true);

        _postprocessors.push_back(postprocess);
    }
}

void
RenderPipeline::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
    auto it = std::find(_postprocessors.begin(), _postprocessors.end(), postprocess);
    if (it != _postprocessors.end())
    {
        postprocess->setActive(false);

        _postprocessors.erase(it);
    }
}

void
RenderPipeline::render() noexcept
{
    assert(_camera);

    RenderListener* renderListener = _camera->getRenderListener();
    if (renderListener)
        renderListener->onWillRenderObject();

    _renderDataManager.clear();

    this->assignLight();
    this->assignVisiable();

    this->onRenderPre();
    this->onRenderPipeline();

    if (_camera->getCameraOrder() == CameraOrder::CO_MAIN)
    {
        auto renderTexture = _camera->getRenderTexture();

        for (auto& it : _postprocessors)
        {
            it->render(*this, renderTexture);
        }

        this->copyRenderTexture(renderTexture, this->getCamera()->getViewport(), 0, this->getCamera()->getViewport());
    }

    this->onRenderPost();

    if (renderListener)
        renderListener->onRenderObject();
}

void
RenderPipeline::assignVisiable() noexcept
{
    assert(_camera);

    RenderScene* scene = _camera->getRenderScene();
    if (scene)
    {
        _visiable.clear();

        scene->computVisiable(_camera, _visiable);

        for (auto& it : _visiable)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            it->collection(_renderDataManager);
        }
    }
}

void
RenderPipeline::assignLight() noexcept
{
    RenderScene* scene = this->getCamera()->getRenderScene();
    if (scene)
    {
        _lights.clear();

        scene->computVisiableLight(this->getCamera(), _lights);

        for (auto& it : _lights)
        {
            auto listener = it->getRenderListener();
            if (listener)
                listener->onWillRenderObject();

            it->collection(_renderDataManager);
        }
    }
}

void
RenderPipeline::onRenderPre() noexcept
{
}

void
RenderPipeline::onRenderPost() noexcept
{
}

void
RenderPipeline::onRenderPipeline() noexcept
{
}

_NAME_END