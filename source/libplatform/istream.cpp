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
#include <ray/istream.h>

_NAME_BEGIN

StreamReader::isentry::isentry(StreamReader* _istr)
	: _ok(true)
	, _my_istr(_istr)
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->lock();
}

StreamReader::isentry::~isentry() noexcept
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->unlock();
}

StreamReader::isentry::operator bool() const noexcept
{
	return _ok ? true : false;
}

StreamReader::StreamReader(StreamBuf* buf) noexcept
	: _count(0)
{
	StreamBase::_init(buf, ios_base::in);
}

StreamReader::~StreamReader() noexcept
{
}

StreamReader&
StreamReader::read(char* str, std::streamsize cnt) noexcept
{
	try
	{
		if (cnt == 0)
			return *this;

		ios_base::iostate state = ios_base::goodbit;

		const isentry ok(this);

		if (ok)
		{
			_count = this->rdbuf()->read(str, cnt);

			if (_count != cnt)
				state |= ios_base::eofbit;
		}

		this->setstate(state);
	}
	catch (...)
	{
		this->setstate(ios_base::badbit, true);
	}

	return (*this);
}

StreamReader&
StreamReader::read(char* str, streamsize size, streamsize cnt) noexcept
{
	return this->read(str, size * cnt);
}

StreamReader&
StreamReader::flush() noexcept
{
	assert(this->rdbuf() != 0);

	const isentry ok(this);
	if (ok)
	{
		if (ok && this->rdbuf()->flush() == -1)
			this->setstate(ios_base::badbit, true);
	}

	return (*this);
}

StreamReader&
StreamReader::seekg(ios_base::off_type pos) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
			this->setstate(ios_base::failbit);
	}

	return (*this);
}

StreamReader&
StreamReader::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
			this->setstate(ios_base::failbit);
	}

	return (*this);
}

streamsize
StreamReader::size() noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->fail())
			return this->rdbuf()->size();
	}

	return (streamsize)ios_base::_BADOFF;
}

streamoff
StreamReader::tellg() noexcept
{
	const isentry ok(this);

	if (!this->fail())
		return (this->rdbuf()->tellg());
	else
		return (ios_base::pos_type(ios_base::_BADOFF));
}

streamsize
StreamReader::gcount() const noexcept
{
	return _count;
}

_NAME_END