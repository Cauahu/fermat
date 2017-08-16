#ifndef FERMAT_COMMON_TIMESPAN_H_
#define FERMAT_COMMON_TIMESPAN_H_
#include <fermat/common/timestamp.h>
#include <algorithm>

namespace fermat {

class Timespan {
public:
    typedef Timestamp::TimeDiff  TimeDiff;
public: 
    /*!
     * create a zero span.
     */
    Timespan();
    explicit Timespan(TimeDiff microSecond);
    Timespan(long sec, long microSec);
    Timespan(int days, int hours, int min, int sec, int microSec);
    Timespan(const Timespan &rhs);
    ~Timespan();

    Timespan& operator = (const Timespan &rhs);
    Timespan& operator = (const TimeDiff microSec);

    Timespan& assign(int days, int hours, int min, int sec, int microSec);

    Timespan& assign(long sec, long microSec);

    void swap(Timespan &other);

    bool operator == (const Timespan& ts) const;
	bool operator != (const Timespan& ts) const;
	bool operator >  (const Timespan& ts) const;
	bool operator >= (const Timespan& ts) const;
	bool operator <  (const Timespan& ts) const;
	bool operator <= (const Timespan& ts) const;

	bool operator == (TimeDiff microSeconds) const;
	bool operator != (TimeDiff microSeconds) const;
	bool operator >  (TimeDiff microSeconds) const;
	bool operator >= (TimeDiff microSeconds) const;
	bool operator <  (TimeDiff microSeconds) const;
	bool operator <= (TimeDiff microSeconds) const;

    operator bool () const;

    Timespan operator + (const Timespan& d) const;
	Timespan operator - (const Timespan& d) const;
	Timespan& operator += (const Timespan& d);
	Timespan& operator -= (const Timespan& d);


	Timespan operator + (TimeDiff microSeconds) const;
	Timespan operator - (TimeDiff microSeconds) const;
    Timespan operator * (int times) const;
    Timespan operator / (int times) const;
	Timespan& operator += (TimeDiff microSeconds);
	Timespan& operator -= (TimeDiff microSeconds);
    Timespan& operator *= (int times);
    Timespan& operator /= (int times);


    int days() const;

    int hours() const;

    int total_hours() const;

    int minutes() const;

    int total_minutes() const;

    int seconds() const;

    int total_seconds() const;

    int mill_seconds() const;

    TimeDiff total_mill_seconds() const;

    int micro_seconds() const;

    int micro_mill_seconds() const;

    TimeDiff total_micro_seconds() const;  
public:
    static const TimeDiff kMillSeconds = 1000L;
    static const TimeDiff kSeconds = 1000L * 1000L;
    static const TimeDiff kMinutes = 1000L * 1000L * 60L;
    static const TimeDiff kHours   = 1000L * 1000L * 60L * 60L;
    static const TimeDiff kDays    = 1000L * 1000L * 60L * 60L * 24L;
private:
    TimeDiff  _span;
};
/* inlines */

inline int Timespan::days() const 
{
    return static_cast<int>(_span / kDays);
}

inline int Timespan::hours() const
{
    return static_cast<int>(_span / kHours % 24);
}

inline int Timespan::total_hours() const
{
    return static_cast<int>(_span / kHours);
}

inline int Timespan::minutes() const 
{
    return static_cast<int>(_span / kMinutes % 60);
}

inline int Timespan::total_minutes() const 
{
    return static_cast<int>(_span / kMinutes);
}

inline int Timespan::seconds() const
{
    return static_cast<int>(_span / kSeconds % 60);
}

inline int Timespan::total_seconds() const 
{
    return static_cast<int>(_span / kSeconds);
}

inline int Timespan::mill_seconds() const
{
    return static_cast<int>(_span / kMillSeconds % 1000);
}

inline Timespan::TimeDiff Timespan::total_mill_seconds() const
{
    return _span / kMillSeconds;
}

inline  int Timespan::micro_seconds() const
{
    return static_cast<int>(_span % 1000000);
}

inline int Timespan::micro_mill_seconds() const
{
    return static_cast<int>(_span %1000);
}

inline  Timespan::TimeDiff Timespan::total_micro_seconds() const
{
    return _span;
}

inline void Timespan::swap(Timespan &other)
{
    if(&other != this) {
        std::swap(this->_span, other._span);
    }
}

inline Timespan& Timespan::operator = (const Timespan &rhs)
{
    if(&rhs != this) {
        this->_span = rhs._span;
    }
    return *this;
}

inline Timespan& Timespan::operator = (const TimeDiff microSec)
{
    this->_span = microSec;
    return *this;
}


inline Timespan& Timespan::assign(int day, int hour, int min, int sec, int microSec)
{
    this->_span = day * kDays +
                  hour * kHours +
                  min * kMinutes +
                  sec * kSeconds + 
                  microSec;
    return *this;
}

inline Timespan& Timespan::assign(long sec, long microSec)
{
    this->_span = sec * kSeconds + microSec;
    return *this;
}

inline bool Timespan::operator == (const Timespan& ts) const
{
    return this->_span == ts._span;
}

inline bool Timespan::operator != (const Timespan& ts) const
{
    return this->_span != ts._span;
}

inline bool Timespan::operator >  (const Timespan& ts) const
{
    return this->_span > ts._span;
}

inline bool Timespan::operator >= (const Timespan& ts) const
{
    return this->_span >= ts._span;
}

inline bool Timespan::operator <  (const Timespan& ts) const
{
    return this->_span < ts._span;    
}

inline bool Timespan::operator <= (const Timespan& ts) const
{
    return this->_span <= ts._span;
}

inline bool Timespan::operator == (TimeDiff microSeconds) const
{
    return this->_span == microSeconds;
}

inline bool Timespan::operator != (TimeDiff microSeconds) const
{
    return this->_span != microSeconds;
}

inline bool Timespan::operator >  (TimeDiff microSeconds) const
{
    return this->_span > microSeconds;
}

inline bool Timespan::operator >= (TimeDiff microSeconds) const
{
     return this->_span >= microSeconds;
}

inline bool Timespan::operator <  (TimeDiff microSeconds) const
{
     return this->_span < microSeconds;
}

inline bool Timespan::operator <= (TimeDiff microSeconds) const
{
     return this->_span <= microSeconds;
}

inline Timespan::operator bool () const
{
    return _span >= 0;
}

inline void swap(Timespan &t1, Timespan &t2)
{
    t1.swap(t2);
}
} //namespace fermat
#endif //FERMAT_COMMON_TIMESPAN_H_
