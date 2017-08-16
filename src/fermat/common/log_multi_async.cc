#include <fermat/common/log_multi_async.h>
#include <iostream>

namespace fermat {

__thread int myindex = -1;

LogMultiAsync::LogMultiAsync(const std::string &baseName,
             size_t rollSize,
             size_t queues,
             int flushInterval)
    : LogOutput("async_log"),
      _flush_interval(flushInterval),
      _is_running(false),
      _base_name(baseName),
      _roll_size(rollSize),
      _state(0),
      _notify(queues+1),
      _mutexs(),
      _current_buffer(),
      _next_buffer(),
      _buffers(),
      _thread("async-log")
{
    _current_buffer.reserve(queues);
    _next_buffer.reserve(queues);
    _buffers.reserve(queues);
}

LogMultiAsync::~LogMultiAsync()
{

}
int LogMultiAsync::get_thread_index()
{
    static std::atomic<int> g_index;
    if(__builtin_expect(myindex == -1, 0)) {
        myindex = g_index++;
    }
    return myindex;
}
void LogMultiAsync::puts(const char* line, size_t len)
{
    if(!_is_running) {
        return ;
    }
    int index = get_thread_index();
    ScopedMutex lock(_mutexs[index]);
  	if ((_current_buffer[index]->size() + len) < _current_buffer[index]->stack_size()) {
	    _current_buffer[index]->append(line, len);
  	} else {
    	_buffers[index].push_back(_current_buffer[index]);
		_current_buffer[index].reset();
    	if (_next_buffer[index])  {
     		_current_buffer[index] = _next_buffer[index];
	  		_next_buffer[index].reset();
		} else {
      		_current_buffer[index].reset(new Buffer); // Rarely happens
		}	
    	_current_buffer[index]->append(line, len);
   	 	_notify.set_to(index);
  	}
}


void LogMultiAsync::run()
{
    _state.set_to(1);
    std::cout<<"start async log"<<std::endl;
    LogFile<NullMutex> output(_base_name, _roll_size);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    size_t max_queue = _buffers.size();
    size_t index = 0;
    size_t emptyQueues = 0;
    while (_is_running) {
        assert(newBuffer1 && newBuffer1->size() == 0);
        assert(newBuffer2 && newBuffer2->size() == 0);
        assert(buffersToWrite.empty());

        {
            index %= max_queue;
            if(emptyQueues >= max_queue) {
                bool ret = _notify.wait_less(max_queue + 1, Timespan(_flush_interval*1000000));
                if(ret) {
                    index = _notify.get();
                    _notify.set_to(max_queue + 1);
                    emptyQueues = 0;
                }
            }
            ScopedMutex lock(_mutexs[index]);
            if(_buffers[index].empty()) {
                emptyQueues++;   
            }
            _buffers[index].push_back(_current_buffer[index]);
            _current_buffer[index] = newBuffer1;
	        newBuffer1.reset();
            buffersToWrite.swap(_buffers[index]);
            if (!_next_buffer[index]) {
                _next_buffer[index] = newBuffer2;
		        newBuffer2.reset();
            }
            ++index;
        }
        
        size_t s = buffersToWrite.size();
        for (size_t i = 0; i < s; ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->size());
        }

        buffersToWrite.resize(2);

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite[0];
            newBuffer1->bzero();
        }

        if (!newBuffer2) {
            assert(buffersToWrite.size() == 2);
            newBuffer2 = buffersToWrite[1];
            newBuffer2->bzero();
        }

        buffersToWrite.clear();
        output.flush();
    } //while
  
    flush_all(&output);
    _state.set_to(2);
}


void LogMultiAsync::flush()
{

}
void LogMultiAsync::flush_all(LogFile<NullMutex> *output)
{
    size_t idx = 0;
    while(idx < _buffers.size()) {
        ScopedMutex lock(_mutexs[idx]);
        _buffers[idx].push_back(_current_buffer[idx]);
        _current_buffer[idx].reset();
        if (!_buffers[idx].empty()) {
            for (size_t i = 0; i <_buffers.size(); i++) {
                output->append(_buffers[idx][i]->data(), _buffers[idx][i]->size());
            }
        }
        output->flush();
    }
}

bool LogMultiAsync::start()
{
    if(_is_running) {
        return true;
    }
     _is_running = true;
    _thread.start(std::bind(&LogMultiAsync::run, this));
    _state.wait_for(1);
    return true;
}

void LogMultiAsync::stop()
{
    if(!_is_running) {
        return;
    }
    _is_running = false;
    _state.wait_for(2);
    _thread.join();
}


}