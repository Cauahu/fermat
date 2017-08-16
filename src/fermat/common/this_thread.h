#ifndef FERMAT_COMMON_THISTHREAD_H_
#define FERMAT_COMMON_THISTHREAD_H_
#include <pthread.h>
#include <fermat/common/timestamp.h>
#include <fermat/common/timespan.h>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>

namespace fermat {
namespace this_thread {

extern __thread pid_t t_tid;
extern __thread char  t_tid_string[32];
extern __thread char  t_tid_string_len;

void cache_thread_id();

inline pid_t thread_id()
{
    if (__builtin_expect(t_tid == 0, 0)) {
      cache_thread_id();
    }
    return t_tid;
}

void set_thread_name(pthread_t thread, const std::string &threadName);

/*!
* @return true if the thread is main thread pid = thread_id
*/
bool is_main_thread();

/*!
* Suspends the current thread for the specified
* amount of time.
*/
bool sleep_for(Timespan span);

/*!
* Suspends the current thread for the specified
* amount of time, a another way to sleep for sleep.
*/
bool sleep_until(Timestamp stamp);

    /*!
    * Yields cpu to other threads.
    */
void yield();
	  /*!
	  * exit current thread.
	  */
void exit();

inline const char* thread_id_string()
{
    return t_tid_string;
}

inline int thread_id_string_length()
{
    return t_tid_string_len;
}

}

}
#endif
