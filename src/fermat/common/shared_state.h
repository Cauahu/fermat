#ifndef COMMON_SHARED_STATE_H_
#define COMMON_SHARED_STATE_H_
#include <fermat/common/cond.h>
#include <fermat/common/mutex.h>
#include <cstddef>

namespace fermat {

template <typename T = size_t>

class SharedState {
public:
	typedef T value_type;
public:
	
	explicit SharedState(const T &value):_value(value), _mutex(), _cond()
	{
	}

	void set_to(const T&value)
	{
		ScopedMutex lock(_mutex);
		_value = value;
		_cond.broadcast();
	}

	void wait_for(const T &condition)
	{
		ScopedMutex lock(_mutex);
		if(_value != condition) {
			_cond.wait(_mutex);
		}
	}
	
	bool wait_for(const T &condition, Timespan span)
	{	
		ScopedMutex lock(_mutex);
		if(_value != condition) {
			_cond.wait(_mutex, span);
		}
		return true;
	}

	bool wait_large(const T &condition)
	{
		ScopedMutex lock(_mutex);
		if(_value <= condition) {
			_cond.wait(_mutex);
		}
		return true;	
	}

	bool wait_large(const T &condition, Timespan span)
	{
		ScopedMutex lock(_mutex);
		if(_value <= condition) {
			_cond.wait(_mutex, span);
		}
		return true;	
	}

	bool wait_less(const T &condition, Timespan span)
	{
		ScopedMutex lock(_mutex);
		if(_value >= condition) {
			_cond.wait(_mutex, span);
		}
		return true;	
	}

	bool wait_less(const T &condition)
	{
		ScopedMutex lock(_mutex);
		if(_value >= condition) {
			_cond.wait(_mutex);
		}
		return true;	
	}

	value_type get() 
	{
		ScopedMutex lock(_mutex);
		return _value;
	}
private:
    SharedState(const SharedState &);
    SharedState& operator=(const SharedState&);
private:
	value_type _value;
	Mutex      _mutex;
	Cond       _cond;
	
};
}

#endif