#include <fermat/common/this_thread.h>
#include <cstdio>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace fermat {
namespace this_thread{

__thread pid_t t_tid = 0;
__thread char  t_tid_string[32] = "";
__thread char  t_tid_string_len = 0;	
  
void set_thread_name(pthread_t thread, const std::string &threadName)
{
#if !defined(__APPLE__) && !defined(__MACH__)
    if (::pthread_setname_np(thread, threadName.c_str()) == ERANGE && threadName.size() > 15) {
        std::string truncName(threadName, 0, 7);
        truncName.append("~");
        truncName.append(threadName, threadName.size() - 7, 7);
        pthread_setname_np(thread, truncName.c_str());
    }
#else 
    ::pthread_setname_np(threadName.c_str());
#endif
}

void cache_thread_id()
{
#if !(defined(__APPLE__) && defined(__MACH__))
    if (t_tid == 0) {
        t_tid = static_cast<pid_t>( syscall (SYS_gettid) );
        t_tid_string_len = snprintf(t_tid_string, 31, "%u", t_tid);
    }
#else
    t_tid = 0; 
#endif
}

bool is_main_thread()
{
#if !(defined(__APPLE__) && defined(__MACH__))
    return thread_id() == (pid_t)::getpid();
#else 
   // return thread_id() == static_cast<pthread_t>(::getpid());
    return true;
#endif
}
    
void exit()
{
    ::pthread_exit(NULL);
}
		
void yield()
{
    ::sched_yield();
}

bool sleep_for(Timespan span)
{
    struct timespec ts = { 0, 0 };
    ts.tv_sec = static_cast<time_t>(span.seconds());
    ts.tv_nsec = static_cast<long>(span.micro_seconds() * 1000);
    ::nanosleep(&ts, NULL);
    return true;
}

bool sleep_until(Timestamp stamp)
{
    fermat::Timestamp now;
    if(now >= stamp) {
        return false;
    } 
    fermat::Timespan span(stamp - now);
    return  sleep_for(span);
}

} //namespace this_thread

} //namespace fermat
