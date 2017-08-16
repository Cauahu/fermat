#ifndef FERMAT_COMMON_THREAD_H_
#define FERMAT_COMMON_THREAD_H_
#include <fermat/common/shared_state.h>
#include <fermat/common/mutex.h>
#include <fermat/common/runnable.h>
#include <memory>
#include <functional>
#include <limits.h>
#include <assert.h>
#include <pthread.h>
#include <string>

namespace fermat {

/*!
* This class implements a platform-independent
* wrapper to an operating system thread.
*
* Every Thread object gets a unique (within
* its process) numeric thread ID.
* Furthermore, a thread can be assigned a name.
* The name of a thread can be changed at any time.
*/
class  Thread {
public:
	typedef pthread_t TID;
	typedef std::function<void()> ThreadFactor;
	//! Thread priorities.
	enum Priority {
		PRIO_LOWEST  ,     //!< The lowest thread priority.
		PRIO_LOW     ,    //!< A lower than normal thread priority.
		PRIO_NORMAL  ,    //!< The normal thread priority.
		PRIO_HIGH    ,    //!< A higher than normal thread priority.
		PRIO_HIGHEST      //!< The highest thread priority.
	};

	enum Policy {
		POLICY_DEFAULT = SCHED_OTHER
	};
   //! thread start and stoped status. 
	enum RunState {
		RS_STOP,
		RS_RUNING,
		RS_STOPING
	};

	//!thread sleep or running  
	enum SleepStat {
		SS_WAKE,
		SS_SLEEP
	};

	/*!
	* Creates a thread. Call start() to start it.
	*/
	Thread();
		
	/*!
	*  Creates a named thread. Call start() to start it.
	*/
	Thread(const std::string& name);

	/*!
	*  Destroys the thread.
	*/
	~Thread();

	/*!
	* @return the unique thread ID of the thread.
	*/
	int id() const;

	/*!
	* @return the native thread ID of the thread.
	*/
	TID tid() const; 

	/*!
	* @return the name of the thread.
	*/
	std::string name() const;

	/*!
	* @return the name of the thread.
	*/
	std::string get_name() const;

	/*!
	* Sets the name of the thread.
	*/
	void set_name(const std::string& name);

	/*!
	* Sets the thread's priority.
	*/
	bool set_priority(Priority prio);

	/*!
	* @return the thread's priority.
	*/
	Priority get_priority() const;	

	/*!
	* Sets the thread's priority, using an operating system specific
	* priority value. Use getMinOSPriority() and getMaxOSPriority() to
	* obtain minimum and maximum priority values. Additionally,
	* a scheduling policy can be specified. The policy is currently
	* only used on POSIX platforms where the values SCHED_OTHER (default),
	* SCHED_FIFO and SCHED_RR are supported.
	*/
	bool set_os_priority(int prio, int policy = POLICY_DEFAULT);

	/*!
	* @return the thread's priority, expressed as an operating system
	* specific priority value.
	*
	* May return 0 if the priority has not been explicitly set.
	*/
	int get_os_priority() const;
	
	/*!
	* @return the minimum operating system-specific priority value,
	* which can be passed to setOSPriority() for the given policy.
	*/
	static int get_min_os_priority(int policy = POLICY_DEFAULT);
	
	/*!
	* @return the maximum operating system-specific priority value,
	* which can be passed to setOSPriority() for the given policy..
	*/
	static int get_max_os_priority(int policy = POLICY_DEFAULT);

	/*!
	*Sets the thread's stack size in bytes.
	* Setting the stack size to 0 will use the default stack size.
	* Typically, the real stack size is rounded up to the nearest
	* page size multiple.
	*/
	void set_stack_size(int size);

	/*!
	* Binds the thread to run only on the CPU core with the 
	* given index.
	* 
	* Does nothing if the system does not support CPU affinity for
	* threads.
	*/
	bool set_affinity(int cpu);

	/*!
	* @return the index of the CPU core this thread has been bound to,
	* or -1 if the thread has not been bound to a CPU.
	*/
	int get_affinity() const;
	
	/*!
	* @return the thread's stack size in bytes.
	* If the default stack size is used, 0 is returned.
	*/
	int get_stack_size() const;
	
	/*!
	* Starts the thread with the given target.
	*
	* Note that the given Runnable object must remain
	* valid during the entire lifetime of the thread, as
	* only a reference to it is stored internally.
	*/
	bool start(const RunnablePtr& target);
		
	/*!
	* Starts the thread with the given functor object or lambda.
	*/
	bool start(const ThreadFactor &fn);	

	/*!
	* Waits until the thread completes execution.
	* If multiple threads try to join the same
	* thread, the result is undefined.
	*/
	void join();

	/*!
	* Returns true if the thread is running.
	*/
	bool is_running() const;
	
	/*!
	* Starts an interruptible sleep. When trySleep() is called,
	* the thread will remain suspended until:
	*   - the timeout expires or
	*   - wakeUp() is called
	*
	* Function returns true if sleep attempt was completed, false
	* if sleep was interrupted by a wakeUp() call.
	* A frequent scenario where trySleep()/wakeUp() pair of functions
	* is useful is with threads spending most of the time idle,
	* with periodic activity between the idle times; trying to sleep
	* (as opposed to sleeping) allows immediate ending of idle thread
	* from the outside.
	*
	* The try_sleep() and wake_up() calls should be used with
	* understanding that the suspended state is not a true sleep,
	* but rather a state of waiting for an event, with timeout
	* expiration. This makes order of calls significant; calling
	* wakeUp() before calling trySleep() will prevent the next
	* trySleep() call to actually suspend the thread (which, in
	* some scenarios, may be desirable behavior).
	*/
	static bool try_sleep(Timespan span);

	/*!
	*Wakes up the thread which is in the state of interruptible
	* sleep. For threads that are not suspended, calling this
	* function has the effect of preventing the subsequent
	* trySleep() call to put thread in a suspended state.d 
	*/
	void wake_up();

	/*!
	*  Returns the Thread object for the currently active thread.
	* If the current thread is the main thread, 0 is returned.
	*/
	static Thread* current();

	/*!
	*  @return the native thread ID for the current thread.
	*/
	static TID currentTid();
		
protected:
	//thread function

	static void* thread_fuction(void *data);

	std::string make_name();
		/// Creates a unique name for a thread.

	static int unique_id();
		/// Creates and returns a unique id for a thread.
	class ThreadData;
	typedef std::shared_ptr<ThreadData> ThreadDataPtr;
private:
	Thread(const Thread&);
	Thread& operator = (const Thread&);
	int map_priority(int prio, int plicy);
	int map_priority_reverse(int prio, int plicy);
	friend class PooledThread;
private:
	int64_t                   _id;
	ThreadDataPtr             _threadData;
	std::string               _name;
	SharedState<int>          _sleepState;
	mutable Mutex             _mutex;
};

} //namespace fermat


#endif
