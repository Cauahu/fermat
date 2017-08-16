#include <fermat/common/cond.h>
#include <fermat/common/mutex.h>

namespace fermat{

Cond::Cond()
{
    ::pthread_cond_init(&_cond, NULL);
}
	
Cond::~Cond()
{
    ::pthread_cond_destroy(&_cond);
}
	
void Cond::wait(Mutex& mutex)
{
    ::pthread_cond_wait(&_cond, &mutex._mutex);
}
	
bool Cond::wait(Mutex& mutex, Timespan span)
{

	struct ::timespec ts;
    Timestamp stamp;
    stamp += span;
    ts.tv_sec = stamp.seconds();
    ts.tv_nsec = stamp.micro_seconds();
	int32_t ecode = ::pthread_cond_timedwait(&_cond, &mutex._mutex, &ts);
	if (ecode == 0) return true;
	return false;
}
	
void Cond::signal() 
{
	::pthread_cond_signal(&_cond);
}
	
void Cond::broadcast()
{
	::pthread_cond_broadcast(&_cond);
}

} //namespace fermat
