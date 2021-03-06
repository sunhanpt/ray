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
#include <ray/physics_joint_component.h>
#include <ray/physics_body_component.h>

_NAME_BEGIN

__ImplementSubInterface(PhysicsJointComponent, GameComponent, "PhysicsJoint")

PhysicsJointComponent::PhysicsJointComponent() noexcept
{
}

PhysicsJointComponent::~PhysicsJointComponent() noexcept
{
}

void 
PhysicsJointComponent::setConnectRigidbody(PhysicsBodyComponentPtr body) noexcept
{
	if (_body != body)
	{
		_body = body;

		if (this->getGameObject())
			this->onBodyChange();
	}
}

PhysicsBodyComponentPtr
PhysicsJointComponent::getConnectRigidbody() const noexcept
{
	return _body;
}

void
PhysicsJointComponent::load(iarchive& reader) noexcept
{
}

void
PhysicsJointComponent::save(archivebuf& write) noexcept
{
}

PhysicsBody* 
PhysicsJointComponent::getRawRigidbody() const noexcept
{
	auto body = this->getComponent<PhysicsBodyComponent>();
	if (body)
		return body->getPhysicsBody();
	return nullptr;
}

PhysicsBody*
PhysicsJointComponent::getRawConnectRigidbody() const noexcept
{
	if (_body)
		return _body->getPhysicsBody();
	return nullptr;
}

void
PhysicsJointComponent::onBodyChange() noexcept
{
}

_NAME_END