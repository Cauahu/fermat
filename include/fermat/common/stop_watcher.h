#ifndef FERMAT_COMMON_STOP_WATCHER_H_
#define FERMAT_COMMON_STOP_WATCHER_H_
#include <fermat/common/clock.h>

namespace fermat {

class  StopWatcher {
public:
	StopWatcher();
	~StopWatcher();

	void start();
		
	void stop();
	
	void reset();
		
	void restart();
		
	Clock::ClockDiff elapsed() const;
		
	int elapsed_seconds() const;
	
	static Clock::ClockVal ratio();
	
private:
	StopWatcher(const StopWatcher&);
	StopWatcher& operator = (const StopWatcher&);

	Clock              _start;
	Clock::ClockDiff   _elapsed;
	bool              _running;
};

//
// inlines
//
inline void StopWatcher::start()
{
	if (!_running)
	{
		_start.update();
		_running = true;
	}
}


inline void StopWatcher::stop()
{
	if (_running)
	{
		Clock current;
		_elapsed += current - _start;
		_running = false;
	}
}


inline int StopWatcher::elapsed_seconds() const
{
	return int(elapsed()/ratio());
}


inline Clock::ClockVal StopWatcher::ratio()
{
	return Clock::ratio();
}


}
#endif