#ifndef FERMAT_COMMON_MUTEX_H_
#define FERMAT_COMMON_MUTEX_H_
#include <fermat/common/timestamp.h>
#include <fermat/common/timespan.h>
#include <cstdint>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <cassert>
#include <cmath>

namespace fermat {

enum class MutexType {
    FAST,                ///< no operation
    ERRORCHECK,          ///< check error
    RECURSIVE            ///< allow recursive locking
};

class Mutex{
public:
    friend class Cond;
public:
    Mutex();
    explicit Mutex(MutexType t);
    ~Mutex();

    void lock();

    bool lock_try();

    bool lock_try_for(Timespan ts); 
    
    void unlock();
private:
     Mutex(const Mutex &);
     Mutex& operator=(const Mutex&); 
private:
    pthread_mutex_t _mutex;
};

class NullMutex{
public:
    NullMutex(){ }
    NullMutex(MutexType type) { (void)type; }
    ~NullMutex() { }

    void lock() { }
    void lock_try() { }
    bool lock_try_for(Timespan ts) {(void)ts; return true; }
    void unlock(){ }
};

class TimedMutex{
public: 
    TimedMutex();

    void lock();

    bool lock_try();

    bool lock_try_for(Timespan ts);

    bool lock_try_until(Timestamp tp);

    void unlock();
private:
    pthread_mutex_t _mutex; 
};

template <typename LOCK>
class ScopedLock{
public:
    explicit ScopedLock(LOCK &lock) : _lock(lock)
    {
        _lock.lock();
    }
    ~ScopedLock()
    {
        _lock.unlock();
    }
private:
    LOCK& _lock;
};

typedef ScopedLock<Mutex> ScopedMutex;

template <typename LOCK>
class UniqueLock{
public:
    explicit UniqueLock(LOCK &l) : _owned(false), _mutex(l)
    {
        _mutex.lock();
    }

    ~UniqueLock()
    {
        if(_owned) {
            _mutex.unlock();
            _owned = false;
        }
    }

    void lock()
    {
        if(!_owned) {
            _mutex.lock();
            _owned = true;
        }
    }

    void unlock()
    {
        if(_owned) {
            _mutex.unlock();
            _owned = false;
        }   
    }

    bool lock_try()
    {
        if(_owned) {
            return true;
        }

        if(_mutex.lock_try()) {
            _owned = true;
        }
        return false;
    }

    bool lock_try_for(Timespan tp)
    {
        if(_owned) {
            return true;
        }

        if(_mutex.lock_try_for(tp)) {
            _owned = true;
        }
        return false;
    }

    bool lock_try_for(Timestamp ts)
    {
        if(_owned) {
            return true;
        }

        if(_mutex.lock_try_for(Timespan(ts - Timestamp()))) {
            _owned = true;
        }
        return false;
    }
private:
    bool    _owned;
    Mutex  &_mutex;
};
}
#endif
