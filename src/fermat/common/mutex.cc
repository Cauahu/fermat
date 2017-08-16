#include <fermat/common/mutex.h>

namespace fermat {
Mutex::Mutex() 
{
	if (::pthread_mutex_init(&_mutex, NULL) != 0) {
	  	assert(false);
	}
}
	
Mutex::Mutex(MutexType type) 
{
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr) ;
    switch (type) {
    case MutexType::FAST: {
        break;
    }
    case MutexType::ERRORCHECK: {
        ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        break;
    }
    case MutexType::RECURSIVE: {
        ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        break;
    }
    }
    if (::pthread_mutex_init(&_mutex, &attr) !=0) {
        assert(false);
    }
}
	
Mutex::~Mutex() 
{
	  ::pthread_mutex_destroy(&_mutex);
}
	
void Mutex::lock() 
{
	if (::pthread_mutex_lock(&_mutex) != 0) {
	    assert(false);
	}
}

bool Mutex::lock_try() 
{
	int32_t ecode = ::pthread_mutex_trylock(&_mutex);
	    if (ecode == 0) {
            return true;
        }
	return false;
}
	
bool Mutex::lock_try_for(Timespan span)
{
#if !defined(__APPLE__) && !defined(__MACH__)
    struct ::timespec ts;
    Timestamp tp;
    tp += span;
    ts.tv_sec = tp.seconds();
    ts.tv_nsec = tp.micro_seconds() * 1000;
    int32_t ecode = ::pthread_mutex_timedlock(&_mutex, &ts);
    if (ecode == 0) return true;
#endif
    return false;
}
		
void Mutex::unlock()
{
    ::pthread_mutex_unlock(&_mutex);
}


TimedMutex::TimedMutex()
{
    if (::pthread_mutex_init(&_mutex, NULL) != 0) {
	  	assert(false);
	}
}

void TimedMutex::lock()
{
    if (::pthread_mutex_lock(&_mutex) != 0) {
	    assert(false);
	}
}

bool TimedMutex::lock_try()
{
    int32_t ecode = ::pthread_mutex_trylock(&_mutex);
	if (ecode == 0) {
        return true;
    }
	return false;    
}

bool TimedMutex::lock_try_for(Timespan span)
{
#if !defined(__APPLE__) && !defined(__MACH__)
    struct ::timespec ts;
    Timestamp tp;
    tp += span;
    ts.tv_sec = tp.seconds();
    ts.tv_nsec = tp.micro_seconds() * 1000;
    int32_t ecode = ::pthread_mutex_timedlock(&_mutex, &ts);
    if (ecode == 0) return true;
#endif
    return false;    
}

bool TimedMutex::lock_try_until(Timestamp tp)
{
#if !defined(__APPLE__) && !defined(__MACH__)
    struct ::timespec ts;
    ts.tv_sec = tp.seconds();
    ts.tv_nsec = tp.micro_seconds() * 1000;
    int32_t ecode = ::pthread_mutex_timedlock(&_mutex, &ts);
    if (ecode == 0) return true;
#endif
    return false;    
}

void TimedMutex::unlock()
{
    ::pthread_mutex_unlock(&_mutex);
}

}//namespace fermat
