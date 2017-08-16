#include <fermat/common/timespan.h>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-const-variable"
#endif

namespace fermat {

Timespan::Timespan() : _span(0)
{

}

Timespan::Timespan(TimeDiff microSecond) :_span(microSecond)
{

}

Timespan::Timespan(long sec, long microSec) : _span(sec * kSeconds + microSec)
{

}

Timespan::Timespan(int day, int hour, int min, int sec, int microSec)
{
    this->_span = day * kDays +
                  hour * kHours +
                  min * kMinutes +
                  sec * kSeconds + 
                  microSec;
}
Timespan::Timespan(const Timespan &rhs)
{
    _span = rhs._span;
}

Timespan::~Timespan()
{

}

Timespan Timespan::operator + (const Timespan& d) const
{
    return Timespan(_span + d._span);
}

Timespan Timespan::operator - (const Timespan& d) const
{
    return Timespan(_span - d._span);
}

Timespan& Timespan::operator += (const Timespan& d)
{
    this->_span += d._span;
    return *this;
}

Timespan& Timespan::operator -= (const Timespan& d)
{
    this->_span -= d._span;
    return *this;
}

Timespan Timespan::operator + (TimeDiff microSeconds) const
{
    return Timespan(_span + microSeconds);
}
Timespan Timespan::operator - (TimeDiff microSeconds) const
{
    return Timespan(_span - microSeconds);
}

Timespan& Timespan::operator += (TimeDiff microSeconds)
{
    this->_span += microSeconds;
    return *this;
}

Timespan& Timespan::operator -= (TimeDiff microSeconds)
{
    this->_span -= microSeconds;
    return *this;  
}

Timespan Timespan::operator * (int times) const
{
    return Timespan(_span * times);
}
Timespan Timespan::operator / (int times) const
{
    return Timespan(_span / times);
}

 Timespan& Timespan::operator *= (int times)
 {
    _span *= times;
    return *this;
 }

Timespan& Timespan::operator /= (int times)
{
    _span /= times;
    return *this;
}

}
