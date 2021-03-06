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
#ifndef _H_INPUT_DEVICE_H_
#define _H_INPUT_DEVICE_H_

#include <ray/input_device_base.h>

#include <queue>
#include <thread>
#include <condition_variable>

_NAME_BEGIN

class EXPORT DefaultInputDevice : public InputDevice
{
	__DeclareSubInterface(DefaultInputDevice, InputDevice)
public:
	DefaultInputDevice() noexcept;
	virtual ~DefaultInputDevice() noexcept;

	virtual void setCaptureObject(CaptureObject window) noexcept;
	virtual CaptureObject getCaptureObject() const noexcept;

	virtual void enableEventPosting(bool enable) noexcept;
	virtual bool enableEventPosting() const noexcept;

	virtual void addInputListener(InputListenerPtr& listener) noexcept;
	virtual void addInputListener(InputListenerPtr&& listener) noexcept;
	virtual void removeInputListener(InputListenerPtr& listener) noexcept;
	virtual void removeInputListener(InputListenerPtr&& listener) noexcept;
	virtual void clearInputListener() noexcept;

	virtual bool sendEvent(const InputEvent& event) noexcept;
	virtual bool postEvent(const InputEvent& event) noexcept;

	virtual bool peekEvents(InputEvent& event) noexcept;
	virtual bool pollEvents(InputEvent& event) noexcept;
	virtual bool waitEvents(InputEvent& event) noexcept;
	virtual bool waitEvents(InputEvent& event, int timeout) noexcept;
	virtual void flushEvent() noexcept;

	virtual InputDevicePtr clone() const noexcept;

private:
	DefaultInputDevice(const DefaultInputDevice&) noexcept = delete;
	DefaultInputDevice& operator=(const DefaultInputDevice&) noexcept = delete;

private:

	typedef std::queue<InputEvent>  InputEvents;
	typedef std::vector<InputListenerPtr> InputListeners;

	InputEvents _events;
	InputListeners _inputListeners;

	bool _enableEventPosting;

	std::mutex _mutex;
	std::condition_variable _dispose;
};

_NAME_END

#endif