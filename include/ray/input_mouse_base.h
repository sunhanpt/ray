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
#ifndef _H_INPUT_MOUSE_BASE_H_
#define _H_INPUT_MOUSE_BASE_H_

#include <ray/input_controller.h>
#include <ray/input_button.h>

_NAME_BEGIN

class EXPORT InputMouse : public InputController
{
	__DeclareSubInterface(InputMouse, InputController)
public:
	InputMouse() noexcept;
	virtual ~InputMouse() noexcept;

	virtual void lockMouse() noexcept = 0;
	virtual void unlockMouse() noexcept = 0;
	virtual bool isLockedMouse() const noexcept = 0;

	virtual void showMouse() noexcept = 0;
	virtual void hideMouse() noexcept = 0;
	virtual bool isShowMouse() noexcept = 0;

	virtual float getAxis(InputAxis::Code axis) const noexcept = 0;

	virtual void setPosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept = 0;
	virtual void getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept = 0;

	virtual bool isButtonDown(InputButton::Code key) const noexcept = 0;
	virtual bool isButtonUp(InputButton::Code key) const noexcept = 0;
	virtual bool isButtonPressed(InputButton::Code key) const noexcept = 0;

	virtual InputMousePtr clone() const noexcept = 0;

private:
	InputMouse(const InputMouse&) noexcept = delete;
	InputMouse& operator=(const InputMouse&) noexcept = delete;
};

_NAME_END

#endif