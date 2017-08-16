#ifndef FERMAT_COMMON_LOG_STREAM_H_
#define FERMAT_COMMON_LOG_STREAM_H_
#include <fermat/common/stack_buffer.h>
#include <fermat/common/string_ref.h>
#include <fermat/common/string.h>
#include <fermat/common/numeric_string.h>
#include <string>
namespace fermat {

class LogStream{
public:
    static const int kBufferSize = 2048;
    typedef StackBuffer<char, kBufferSize> Buffer;
    typedef LogStream self;
public:
    LogStream(){}
    ~LogStream(){}

    self& operator << (bool v)
    {
        static const char *t = "1";
        static const char *f = "0";
        if(v) {
            _buffer.append(t, t+1);
        } else {
            _buffer.append(f, f+1);   
        }
        return *this;
    }

    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);
    self& operator<<(const void*);

    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }

    self& operator<<(double);

    self& operator<<(char v)
    {
        _buffer.append(&v, 1);
        return *this;
    }

    self& operator<<(const char* v)
    {
        _buffer.append(v, strlen(v));
        return *this;
    }

    self& operator<<(const std::string& v)
    {
        _buffer.append(v.c_str(), v.size());
        return *this;
    }

    self& operator<<(const StringRef& v)
    {
        _buffer.append(v.data(), v.size());
        return *this;
    }

    void append(const char* data, size_t len)
    {
        _buffer.append(data, len);
    }
    const Buffer& buffer() const { return _buffer; }
    void reset_bufffer() { _buffer.clear(); }
private:
    template <typename T>
    void append_int(T value) 
    {
        std::string ret;
        uint_to_str(value, 0x10, ret);
        _buffer.append(ret.data(), ret.length());
    }
private:
    Buffer _buffer;
};

}
#endif