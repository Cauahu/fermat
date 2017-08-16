#include <fermat/common/this_thread.h>
#include <fermat/common/logging.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>

namespace fermat {
__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    ::strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
    return t_errnobuf;
}

Logging::LogLevel init_log_level()
{
  if (::getenv("FERMAT_LOG_TRACE")) {
   	 return Logging::eTRACE;
  } else if (::getenv("FERMAT_LOG_DEBUG")) {
    return Logging::eDEBUG;
  } else {
    return Logging::eINFO;
  }
}

Logging::LogLevel g_log_level = init_log_level();

const char* LogLevelName[Logging::eNUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

class T {
 public:
  T(const char* str, unsigned len)
    :_str(str),
     _len(len)
  {
    assert(strlen(str) == _len);
  }

  const char*    _str;
  const unsigned _len;
};

inline LogStream& operator<<(LogStream& s, T v)
{
  s.append(v._str, v._len);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logging::SourceFile& v)
{
  s.append(v._data, v._size);
  return s;
}

class DefaultOutPut : public LogOutput {
public:
    DefaultOutPut():LogOutput("default_log_output") {}
    ~DefaultOutPut() {}
    virtual void puts(const char* buf, size_t len)
    {
        size_t n = fwrite(buf, 1, len, stdout);
        (void)n;   
    }

    virtual void flush()
    {
        fflush(stdout);   
    }
};
LogOutputPtr g_output(new DefaultOutPut());

Logging::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : _time(),
    _stream(),
    _level(level),
    _line(line),
    _basename(file)
{
   
    format_time();
    this_thread::thread_id();

    _stream << T(this_thread::thread_id_string(),this_thread::thread_id_string_length())<<" ";
    _stream << T(LogLevelName[level], 6);
    _stream << " [" << _basename << ':' << _line << "] ";
    if (savedErrno != 0) {
        _stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}

void Logging::Impl::format_time()
{
    int64_t microSecondsSinceEpoch = _time.total_micro_seconds();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
    if (seconds != t_lastSecond) {
        t_lastSecond = seconds;
        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time);
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17); (void)len;
    }
    char us[32];
    snprintf(us, (sizeof(us) -1), ".%06dZ ", microseconds);
    _stream << T(t_time, 17) << T(us, 9);
}

void Logging::Impl::finish()
{
    _stream <<'\n';
}

Logging::Logging(SourceFile file, int line)
  : _impl(eINFO, 0, file, line)
{
}

Logging::Logging(SourceFile file, int line, LogLevel level, const char* func)
  : _impl(level, 0, file, line)
{
  _impl._stream << func << ' ';
}

Logging::Logging(SourceFile file, int line, LogLevel level)
  : _impl(level, 0, file, line)
{
}

Logging::Logging(SourceFile file, int line, bool toAbort)
  : _impl(toAbort?eFATAL:eERROR, errno, file, line)
{
}

Logging::~Logging()
{
    _impl.finish();/*
    const char * data = stream().buffer().data();
    size_t size = stream().buffer().size();*/
    const LogStream::Buffer& buf(stream().buffer());
    LogOutputPtr out = g_output;
    out->puts(buf.data(), buf.size());
    if (_impl._level == eFATAL){
            out->flush();
    }
}

void Logging::set_log_level(Logging::LogLevel level)
{
    g_log_level = level;
}

void Logging::set_output(LogOutputPtr &out)
{
    g_output = out;
}


}