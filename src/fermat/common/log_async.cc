#include <fermat/common/log_async.h>
#include <iostream>

namespace fermat {
LogAsync::LogAsync(const std::string &baseName,
             size_t rollSize,
             int flushInterval)
    : LogOutput("async_log"),
      _flush_interval(flushInterval),
      _is_running(false),
      _base_name(baseName),
      _roll_size(rollSize),
      _state(0),
      _mutex(),
      _cond(),
      _current_buffer(new Buffer()),
      _next_buffer(new Buffer()),
      _buffers(),
      _thread("async-log")
{
    _current_buffer->bzero();
    _next_buffer->bzero();
    _buffers.reserve(16);
}

LogAsync::~LogAsync()
{

}
void LogAsync::puts(const char* line, size_t len)
{
    if(!_is_running) {
        return ;
    }
    ScopedMutex lock(_mutex);
  	if ((_current_buffer->size() + len) < _current_buffer->stack_size()) {
	    _current_buffer->append(line, len);
  	} else {
    	_buffers.push_back(_current_buffer);
		_current_buffer.reset();
    	if (_next_buffer)  {
     		_current_buffer = _next_buffer;
	  		_next_buffer.reset();
		} else {
      		_current_buffer.reset(new Buffer); // Rarely happens
		}	
    	_current_buffer->append(line, len);
   	 	_cond.signal();
  	}
}

void LogAsync::run()
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
    while (_is_running) {
        assert(newBuffer1 && newBuffer1->size() == 0);
        assert(newBuffer2 && newBuffer2->size() == 0);
        assert(buffersToWrite.empty());

        {
            ScopedMutex lock(_mutex);
            if (_buffers.empty()) {
                _cond.wait(_mutex, Timespan(_flush_interval*1000000));
            }
            _buffers.push_back(_current_buffer);
            _current_buffer = newBuffer1;
	        newBuffer1.reset();
            buffersToWrite.swap(_buffers);
            if (!_next_buffer) {
                _next_buffer = newBuffer2;
		        newBuffer2.reset();
            }
        }
        /*
        if (buffersToWrite.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
               Timestamp().data_time().c_str(),
               buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
	        buffersToWrite.resize(2);
        }
        */
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

void LogAsync::flush()
{

}
void LogAsync::flush_all(LogFile<NullMutex> *output)
{
    ScopedMutex lock(_mutex);
	_buffers.push_back(_current_buffer);
	_current_buffer.reset();
	if (!_buffers.empty()) {
	    for (size_t i = 0; i <_buffers.size(); i++) {
		    output->append(_buffers[i]->data(), _buffers[i]->size());
		}
	}
	output->flush();
}

bool LogAsync::start()
{
    if(_is_running) {
        return true;
    }
     _is_running = true;
    _thread.start(std::bind(&LogAsync::run, this));
    _state.wait_for(1);
    return true;
}

void LogAsync::stop()
{
    if(!_is_running) {
        return;
    }
    _is_running = false;
    _state.wait_for(2);
    _thread.join();
}

} //namespace fermat