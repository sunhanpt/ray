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
#include <ray/game_base_features.h>
#include <ray/game_object_manager.h>
#include <ray/game_scene_manager.h>

_NAME_BEGIN

__ImplementSubClass(GameBaseFeatures, GameFeature, "GameBaseFeatures")

GameBaseFeatures::GameBaseFeatures() noexcept
{
}

GameBaseFeatures::~GameBaseFeatures() noexcept
{
}

void
GameBaseFeatures::onActivate() except
{
	if (!GameSceneManager::instance()->open())
		throw failure("GameSceneManager::instance() fail.");
}

void
GameBaseFeatures::onDeactivate() noexcept
{
	GameSceneManager::instance()->close();
}

void
GameBaseFeatures::onFrameBegin() noexcept
{
	GameSceneManager::instance()->onFrameBegin();
	GameObjectManager::instance()->onFrameBegin();
}

void
GameBaseFeatures::onFrame() noexcept
{
	GameSceneManager::instance()->onFrame();
	GameObjectManager::instance()->onFrame();
}

void
GameBaseFeatures::onFrameEnd() noexcept
{
	GameSceneManager::instance()->onFrameEnd();
	GameObjectManager::instance()->onFrameEnd();
}

_NAME_END