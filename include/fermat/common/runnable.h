#ifndef FERMAT_COMMON_RUNNABLE_H_
#define FERMAT_COMMON_RUNNABLE_H_
#include <memory>

namespace fermat {

/*!
* The Runnable interface with the run() method
* must be implemented by classes that provide
* an entry point for a thread.
*/
class  Runnable {
public:	
	Runnable();
	virtual ~Runnable();

	/*!
	* Do whatever the thread needs to do. Must
	* be overridden by subclasses.
	*/
	virtual void run() = 0;

};

typedef std::shared_ptr<Runnable> RunnablePtr;

} //namespace fermat

#endif
 