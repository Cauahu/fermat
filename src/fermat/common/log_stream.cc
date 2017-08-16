#include <fermat/common/log_stream.h>
#include <cstdio>
#include <cstdarg>

namespace fermat {

LogStream& LogStream::operator<<(short v)
{
	*this<<static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{		
	*this<<static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
    append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v)
{
	append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) 
{
	append_int(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{	
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    std::string ret;
    uint_to_str(v, 0x10, ret, true,  18, '0');
    _buffer.append(ret.c_str(), ret.length());
	return *this;
}

LogStream& LogStream::operator<<(double v)
{
    std::string result; 
    double_to_str(result, v);
	*this<<result;
	return *this;
}

/*
LogStream& LogStream::sprintf(const char *fmt, ...)
{
	if(_buffer.avaible() <= 2) {
		return *this;
	}
	va_list args;
	va_start(args, fmt);
	int size = vsnprintf(_buffer.current(), _buffer.avaible() -2, fmt, args);
	va_end(args);
	if(size > 0) {
		_buffer.drain(size);
	}
	return *this;
}
*/
}
