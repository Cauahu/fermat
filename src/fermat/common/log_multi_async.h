#ifndef FERMAT_COMMON_LOG_MULTI_ASYNC_H_
#define FERMAT_COMMON_LOG_MULTI_ASYNC_H_

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

class LogMultiAsync : public LogOutput {
public:
LogMultiAsync(const std::string &baseName,
             size_t rollSize,
             size_t queues,
             int flushInterval = 3);
    virtual ~LogMultiAsync();

    virtual void puts(const char* line, size_t len);

    virtual void flush();

    bool start();

    void stop();

    void run();
private:
    void flush_all(LogFile<NullMutex> *out);
    int get_thread_index();
private:
    typedef StackBuffer<char, 4096>   Buffer; 
    typedef std::shared_ptr<Buffer>   BufferPtr;
    typedef std::vector<BufferPtr>    BufferVector;
    typedef std::vector<BufferVector> BufferMatrix;

    const int                        _flush_interval;
    bool                             _is_running;
    std::string                      _base_name;
    size_t                           _roll_size;
    SharedState<int>                 _state;
    SharedState<size_t>                 _notify;
    std::vector<Mutex>               _mutexs;
    BufferVector                     _current_buffer;
    BufferVector                     _next_buffer;
    BufferMatrix                     _buffers;
    Thread                           _thread;
};
}
#endif