#include <fermat/common/clock.h>
#include <algorithm>
#undef min
#undef max
#include <limits>
#if defined(__APPLE__) || defined(__MACH__)
    #include <mach/mach_time.h>
#elif defined(__unix__) || defined(__unix) || defined(unix)
    #include <sys/time.h>
	#include <time.h>
#endif



namespace fermat {

const Clock::ClockVal Clock::kMinClockVal = std::numeric_limits<Clock::ClockVal>::min();
const Clock::ClockVal Clock::kMaxClockVal = std::numeric_limits<Clock::ClockVal>::max();

Clock::Clock()
{
	update();
}


Clock::Clock(ClockVal tv)
{
	_clock = tv;
}


Clock::Clock(const Clock& other)
{
	_clock = other._clock;
}


Clock::~Clock()
{
}


Clock& Clock::operator = (const Clock& other)
{
	_clock = other._clock;
	return *this;
}


Clock& Clock::operator = (ClockVal tv)
{
	_clock = tv;
	return *this;
}


void Clock::swap(Clock& timestamp)
{
	std::swap(_clock, timestamp._clock);
}


void Clock::update()
{
#if defined(__APPLE__) && defined(__MACH__)
    int64_t tp = mach_absolute_time();
    mach_timebase_info_data_t time_info;
    mach_timebase_info(&time_info);
    _clock = tp *time_info.numer / time_info.denom / 1000;
#else 
	struct timespec ts;
	::clock_gettime(CLOCK_MONOTONIC, &ts);
	_clock = ClockVal(ts.tv_sec)*ratio() + ts.tv_nsec/1000;
#endif

}


Clock::ClockDiff Clock::accuracy()
{
#if defined(__APPLE__) && defined(__MACH__)
    return 1000000; //actually 1000000000
#else //linux
	struct timespec ts;
	::clock_getres(CLOCK_MONOTONIC, &ts);
	ClockVal acc = ClockVal(ts.tv_sec)*ratio() + ts.tv_nsec/1000;
	return acc > 0 ? acc : 1;
#endif

}

	
bool Clock::monotonic()
{
	return true;
}

}