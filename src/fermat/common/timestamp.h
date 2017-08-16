#ifndef FERMAT_COMMON_TIMESTAMP_H_
#define FERMAT_COMMON_TIMESTAMP_H_
#include <cstdint>
#include <sys/time.h>
#include <ctime>
#include <string>

namespace fermat {

class Timespan;

class Timestamp {
public:
    typedef int64_t TimeVal;
    typedef int64_t TimeDiff;
    typedef int64_t UtcTimeVal;

    static const TimeVal  kMaxTimeVal;
    static const TimeVal  kMinTimeVal;
    static const TimeDiff kMicroSecPerSec = 1000000;
public:

    Timestamp();

    Timestamp(TimeVal tv);

    Timestamp(const Timestamp & rhs);

    ~Timestamp();

    Timestamp& operator =(const Timestamp &rhs);
    Timestamp& operator =(const TimeVal &rhs);

    void swap(Timestamp &other);

    void update();

    bool operator == (const Timestamp& ts) const;
	bool operator != (const Timestamp& ts) const;
	bool operator >  (const Timestamp& ts) const;
	bool operator >= (const Timestamp& ts) const;
	bool operator <  (const Timestamp& ts) const;
	bool operator <= (const Timestamp& ts) const;
	
	Timestamp  operator +  (TimeDiff d) const;
	Timestamp  operator +  (const Timespan& span) const;
	Timestamp  operator -  (TimeDiff d) const;
	Timestamp  operator -  (const Timespan& span) const;
	TimeDiff   operator -  (const Timestamp& ts) const;
	Timestamp& operator += (TimeDiff d);
	Timestamp& operator += (const Timespan& span);
	Timestamp& operator -= (TimeDiff d);
	Timestamp& operator -= (const Timespan& span);

    /*!
	 * @return the timestamp expressed in time_t.
	 * time_t base time is midnight, January 1, 1970.
	 * ratio is one second.
	 */
    std::time_t seconds() const;

    /*!
	 * @return  the timestamp expressed in microseconds
	 * since the Unix epoch, midnight, January 1, 1970.
	 */	
	TimeVal micro_seconds() const;

    TimeVal mill_seconds() const;

    /*!
     * @return total mico/mill seconds
     */
    TimeVal total_micro_seconds() const;
    TimeVal total_mill_seconds() const;

	std::string data_time();
    /*!
	 * @return the timestamp expressed in UTC-based
	 * time. UTC base time is midnight, October 15, 1582.
	 * Resolution is 100 nanoseconds.
	 */	
    UtcTimeVal utc_time() const;

    

    /*!
	 * @return   the time elapsed since the time denoted by
	 * the timestamp. Equivalent to Timestamp() - *this.
     * ratio in mico second.
	 */	
	TimeDiff elapsed() const;
	
	/*!
	 * @return  true iff the given interval has passed
	 * since the time denoted by the timestamp.
	 */	
	bool is_elapsed(TimeDiff interval) const;
	
	/*!
	 * @return  Creates a timestamp from a std::time_t.
	 */
	static Timestamp make_by_time(std::time_t t);

    static Timestamp make_by_mill_time(std::time_t t);

    static Timestamp make_by_micro_time(std::time_t t);
	
	/*!
	 * @return  Creates a timestamp from a UTC time value
	 * (100 nanosecond intervals since midnight,
	 * October 15, 1582).
	 */
	static Timestamp make_by_utc_time(UtcTimeVal val);
	
	/*!
	 * @return  the resolution in units per second.
	 * Since the timestamp has microsecond resolution,
	 * the returned value is always 1000000.
	 */
	static TimeDiff ratio();    
private:
    TimeVal    _ts;

};


// 
// inlines
//
 inline bool Timestamp::operator == (const Timestamp& ts) const
 {
    return this->_ts == ts._ts;
 }

inline bool Timestamp::operator != (const Timestamp& ts) const
{
    return this->_ts != ts._ts;
}

inline bool Timestamp::operator >  (const Timestamp& ts) const
{
    return this->_ts > ts._ts;
}

inline bool Timestamp::operator >= (const Timestamp& ts) const
{
     return this->_ts >= ts._ts;
}

inline bool Timestamp::operator <  (const Timestamp& ts) const
{
     return this->_ts < ts._ts;
}

inline bool Timestamp::operator <= (const Timestamp& ts) const
{
     return this->_ts <= ts._ts;
}


inline Timestamp::TimeDiff Timestamp::operator -  (const Timestamp& ts) const
{
    return _ts - ts._ts;
}

inline Timestamp& Timestamp::operator += (Timestamp::TimeDiff d)
{
    _ts += d;
    return *this;
}

inline Timestamp& Timestamp::operator -= (Timestamp::TimeDiff d)
{
    _ts -= d;
    return *this;
}

inline std::time_t Timestamp::seconds() const
{
    return static_cast<std::time_t>(_ts / ratio());
}

inline Timestamp::TimeDiff Timestamp::ratio()
{
    return kMicroSecPerSec;
}

inline Timestamp::TimeVal Timestamp::micro_seconds() const
{
    return _ts % ratio();
}

inline Timestamp::TimeVal Timestamp::mill_seconds() const
{
    return (_ts % ratio()) / 1000;
}

inline Timestamp::TimeVal Timestamp::total_micro_seconds() const
{
    return _ts;
}

inline Timestamp::TimeVal Timestamp::total_mill_seconds() const
{
    return _ts / 1000;
}

inline Timestamp::TimeDiff Timestamp::elapsed() const
{
     Timestamp now;
     return now._ts - _ts;
}

inline bool Timestamp::is_elapsed(Timestamp::TimeDiff interval) const
{
    Timestamp now;
	Timestamp::TimeDiff diff = now - *this;
	return diff >= interval;
}
inline Timestamp::UtcTimeVal Timestamp::utc_time() const
{
    return _ts*10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
}

inline Timestamp& Timestamp::operator =(const Timestamp &rhs)
{
	this->_ts = rhs._ts;
	return *this;
}

inline Timestamp& Timestamp::operator =(const TimeVal &rhs)
{
	this->_ts = rhs;
	return *this;
}
} //namespace fermat

#endif //FERMAT_COMMON_TIMESTAMP_H_