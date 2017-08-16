#ifndef FERMAT_COMMON_LOG_ASYNC_H_
#define FERMAT_COMMON_LOG_ASYNC_H_
#include <fermat/common/logging.h>
#include <fermat/common/mutex.h>
#include <fermat/common/thread.h>
#include <fermat/common/shared_state.h>
#include <fermat/common/stack_buffer.h>
#include <fermat/common/log_file.h>
#include <memory>
#include <cstddef>
#include <vector>

namespace fermat {

class LogAsync : public LogOutput {
public:
    LogAsync(const std::string &baseName,
             size_t rollSize,
             int flushInterval = 3);
    virtual ~LogAsync();

    virtual void puts(const char* line, size_t len);

    virtual void flush();

    bool start();

    void stop();

    void run();
private:
    void flush_all(LogFile<NullMutex> *out);
    typedef StackBuffer<char, 4096>   Buffer; 
    typedef std::shared_ptr<Buffer>   BufferPtr;
    typedef std::vector<BufferPtr>    BufferVector;   
    const int                        _flush_interval;
    bool                             _is_running;
    std::string                      _base_name;
    size_t                           _roll_size;
    SharedState<int>                 _state;
    Mutex                            _mutex;
    Cond                             _cond;
    BufferPtr                        _current_buffer;
    BufferPtr                        _next_buffer;
    BufferVector                     _buffers;
    Thread                           _thread;
};
}
#endif
