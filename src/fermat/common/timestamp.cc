#include <fermat/common/timestamp.h>
#include <fermat/common/timespan.h>
#include <sys/time.h>
#include <limits>
#include <sstream>

namespace fermat {

const Timestamp::TimeVal Timestamp::kMaxTimeVal = std::numeric_limits<int64_t>::max();
const Timestamp::TimeVal Timestamp::kMinTimeVal = std::numeric_limits<int64_t>::min();

Timestamp::Timestamp()
{
    update();
}

Timestamp::Timestamp(Timestamp::TimeVal tv) : _ts(tv)
{

}

Timestamp::Timestamp(const Timestamp &other)
{
    _ts = other._ts;
}

Timestamp::~Timestamp()
{

}

void Timestamp::swap(Timestamp &other)
{
    std::swap(_ts, other._ts);
}

Timestamp  Timestamp::operator +  (TimeDiff d) const
{
    return Timestamp(_ts + d);
}
Timestamp  Timestamp::operator +  (const Timespan& span) const
{
    return Timestamp(_ts + span.total_micro_seconds());
}
Timestamp  Timestamp::operator -  (TimeDiff d) const
{
    return Timestamp(_ts - d); 
}
Timestamp  Timestamp::operator -  (const Timespan& span) const
{
    return Timestamp(_ts - span.total_micro_seconds());
}

void Timestamp::update()
{
    struct timeval tv;
	if (gettimeofday(&tv, NULL)) {
		_ts = 0;
		return ;
	}
	_ts = static_cast<TimeVal>(tv.tv_sec) * ratio() + tv.tv_usec;	
}

std::string Timestamp::data_time()
{
    time_t time = static_cast<time_t>(_ts / 1000000);
    struct tm* t;
    struct tm tr;
    localtime_r(&time, &tr);
    t = &tr;

    char buf[32];
    strftime(buf, sizeof(buf), "%F %H:%M:%S", t);
    //strftime(buf, sizeof(buf), "%x %H:%M:%S", t);

    std::ostringstream os;
    os << buf << ".";
    os.fill('0');
	
	os.width(6);
	
    os << static_cast<long>(_ts % 1000000);
    return os.str();
}
Timestamp& Timestamp::operator += (const Timespan& span)
{
    _ts += span.total_micro_seconds();
    return *this;
}
} //namepsace fermat