#include <fermat/common/stop_watcher.h>

namespace fermat {

StopWatcher::StopWatcher(): _elapsed(0), _running(false)
{
}


StopWatcher::~StopWatcher()
{
}


Clock::ClockDiff StopWatcher::elapsed() const
{
	if (_running) {
		Clock current;
		return _elapsed + (current - _start);
	} else {
		return _elapsed;
	}
}


void StopWatcher::reset()
{
	_elapsed = 0;
	_running = false;
}


void StopWatcher::restart()
{
	_elapsed = 0;
	_start.update();
	_running = true;
}

}