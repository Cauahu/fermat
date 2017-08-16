#ifndef FERMAT_COMMON_CLOCK_H_
#define FERMAT_COMMON_CLOCK_H_
#include <cstdint>

namespace fermat {

class Clock {
public: 
    typedef int64_t ClockVal;
    typedef int64_t ClockDiff;

    static const ClockVal kMaxClockVal;
    static const ClockVal kMinClockVal;
public: 
    Clock();

    Clock(ClockVal cv);

    Clock(const Clock &other);

    ~Clock();

    Clock& operator = (const Clock& other);
	Clock& operator = (ClockVal tv);

    void swap(Clock& clock);

    void update();

	bool operator == (const Clock& ts) const;
	bool operator != (const Clock& ts) const;
	bool operator >  (const Clock& ts) const;
	bool operator >= (const Clock& ts) const;
	bool operator <  (const Clock& ts) const;
	bool operator <= (const Clock& ts) const;
	
	Clock  operator +  (ClockDiff d) const;
	Clock  operator -  (ClockDiff d) const;
	ClockDiff operator - (const Clock& ts) const;
	Clock& operator += (ClockDiff d);
	Clock& operator -= (ClockDiff d);

    ClockVal raw() const;

    ClockVal micro_seconds() const;

    ClockDiff elapsed() const;

    bool is_elapsed(ClockDiff interval) const;

    static ClockDiff ratio();

    static bool monotonic();

    static ClockDiff accuracy();

private:
    ClockVal _clock;
}; 

//
// inlines
//
inline bool Clock::operator == (const Clock& ts) const
{
	return _clock == ts._clock;
}


inline bool Clock::operator != (const Clock& ts) const
{
	return _clock != ts._clock;
}


inline bool Clock::operator >  (const Clock& ts) const
{
	return _clock > ts._clock;
}


inline bool Clock::operator >= (const Clock& ts) const
{
	return _clock >= ts._clock;
}


inline bool Clock::operator <  (const Clock& ts) const
{
	return _clock < ts._clock;
}


inline bool Clock::operator <= (const Clock& ts) const
{
	return _clock <= ts._clock;
}


inline Clock Clock::operator + (Clock::ClockDiff d) const
{
	return Clock(_clock + d);
}


inline Clock Clock::operator - (Clock::ClockDiff d) const
{
	return Clock(_clock - d);
}


inline Clock::ClockDiff Clock::operator - (const Clock& ts) const
{
	return _clock - ts._clock;
}


inline Clock& Clock::operator += (Clock::ClockDiff d)
{
	_clock += d;
	return *this;
}


inline Clock& Clock::operator -= (Clock::ClockDiff d)
{
	_clock -= d;
	return *this;
}


inline Clock::ClockVal Clock::micro_seconds() const
{
	return _clock;
}


inline Clock::ClockDiff Clock::elapsed() const
{
	Clock now;
	return now - *this;
}


inline bool Clock::is_elapsed(Clock::ClockDiff interval) const
{
	Clock now;
	Clock::ClockDiff diff = now - *this;
	return diff >= interval;
}


inline Clock::ClockDiff Clock::ratio()
{
	return 1000000;
}


inline void swap(Clock& s1, Clock& s2)
{
	s1.swap(s2);
}


inline Clock::ClockVal Clock::raw() const
{
	return _clock;
}

 
}
#endif