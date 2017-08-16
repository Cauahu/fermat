#ifndef FERMAT_COMMON_LOGGER_H_
#define FERMAT_COMMON_LOGGER_H_
#include <fermat/common/log_stream.h>
#include <fermat/common/timestamp.h>
#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wtautological-compare"
#else
    #pragma GCC diagnostic ignored "-Wtype-limits"
#endif
#include <memory>

namespace fermat {

class LogOutput {
public:
    explicit LogOutput(const std::string& logName) : _log_name(logName) {}
    virtual ~LogOutput(){}
    virtual void puts(const char* buf, size_t len) = 0; 
    virtual void flush() = 0;
protected:
    std::string  _log_name;
};
typedef std::shared_ptr<LogOutput> LogOutputPtr;
class Logging {
public:
    enum LogLevel{
        eTRACE,
        eDEBUG,
        eINFO,
        eWARN,
        eERROR,
        eFATAL,
        eNUM_LOG_LEVELS
    };

    class SourceFile{
    public:
        template<int N>
        inline SourceFile(const char (&arr)[N])
        : _data(arr),
          _size(N-1)
        {
            const char* slash = strrchr(_data, '/'); // builtin function
            if (slash) {
                _data = slash + 1;
                _size -= _data - arr;
            }
        }

        explicit SourceFile(const char* filename)
        : _data(filename)
        {
        const char* slash = strrchr(filename, '/');
        if (slash) {
            _data = slash + 1;
        }
        _size = static_cast<int>(strlen(_data));
        }

        const char* _data;
        size_t      _size;
    };
    
    Logging(SourceFile file, int line);
    Logging(SourceFile file, int line, LogLevel level);
    Logging(SourceFile file, int line, LogLevel level, const char* func);
    Logging(SourceFile file, int line, bool toAbort);
    ~Logging();
    
    LogStream& stream() { return _impl._stream; }

    static LogLevel log_level();
    static void set_log_level(LogLevel level);

    static void set_output(LogOutputPtr &ptr);
private:
    class Impl {
    public:
        typedef Logging::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void format_time();
        void finish();

        Timestamp    _time;
        LogStream    _stream;
        LogLevel     _level;
        int          _line;
        SourceFile   _basename;
    };

    Impl       _impl;

};

extern Logging::LogLevel g_log_level;

inline Logging::LogLevel Logging::log_level()
{
  return g_log_level;
}

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::fermat::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Logging::SourceFile file, int line, const char *names, T* ptr) 
{
    if (ptr == NULL) {
        Logging(file, line, Logging::eFATAL).stream() << names;
    }
    return ptr;
}

} //namespace fermat

#define LOG_TRACE if(fermat::Logging::eTRACE >= fermat::Logging::log_level()) \
    fermat::Logging(__FILE__, __LINE__, fermat::Logging::eTRACE, __func__).stream()
#define LOG_DEBUG if(fermat::Logging::eDEBUG >= fermat::Logging::log_level()) \
    fermat::Logging(__FILE__, __LINE__, fermat::Logging::eDEBUG, __func__).stream()
#define LOG_INFO if(fermat::Logging::eINFO >= fermat::Logging::log_level()) \
    fermat::Logging(__FILE__, __LINE__, fermat::Logging::eINFO,__func__).stream()
#define LOG_WARN if(fermat::Logging::eWARN >= fermat::Logging::log_level()) \
    fermat::Logging(__FILE__, __LINE__, fermat::Logging::e WARN,__func__).stream()
#define LOG_ERROR fermat::Logging(__FILE__, __LINE__, fermat::Logging::eERROR, __func__).stream()
#define LOG_FATAL fermat::Logging(__FILE__, __LINE__, fermat::Logging::eFATAL, __func__).stream()


#endif
