#ifndef FERMAT_COMMON_LOG_FILE_H_
#define FERMAT_COMMON_LOG_FILE_H_
#include <fermat/common/mutex.h>
#include <fermat/common/timestamp.h>
#include <fermat/common/sequence_write_file.h>
#include <string>
#include <memory>

namespace fermat {

template <typename MUTEX>
class LogFile {
public:
   LogFile(const std::string &name, 
           const size_t rollSize,
           const int flushInterval = 3,
           const int checkSize = 4096);

   ~LogFile();
   void append(const char* line, size_t len);
   void append(const std::string &line);
   bool roll();
   void flush();

private:
    void append_unlock(const char *line, size_t len);
    std::string get_log_file_name(const Timestamp &stamp);
private:
    const std::string                  _base_name;
    const size_t                       _roll_size;
    const int                          _flush_step;
    const int                          _check_size;
    int                                _count;
    MUTEX                              _mutex;
    Timestamp                          _last_roll;
    Timestamp                          _last_flush;
    std::shared_ptr<SequenceWriteFile> _file;
};
template <typename MUTEX>
inline LogFile<MUTEX>::LogFile(const std::string &name, 
           const size_t rollSize,
           const int flushInterval,
           const int checkSize)
    : _base_name(name),
      _roll_size(rollSize),
      _flush_step(flushInterval),
      _check_size(checkSize),
      _count(0)
{
    roll();
}
template <typename MUTEX>
inline LogFile<MUTEX>::~LogFile()
{

}

template <typename MUTEX>
inline void LogFile<MUTEX>::append(const char* line, size_t len)
{
    ScopedLock<MUTEX> lock(_mutex);
    append_unlock(line, len);
}

template <typename MUTEX>
inline void LogFile<MUTEX>::append(const std::string &line)
{
    append(line.c_str(), line.length());
}

template <typename MUTEX>
inline bool LogFile<MUTEX>::roll()
{
	Timestamp t;
	std::string filename = get_log_file_name(t);

	if(t.seconds() > _last_roll.seconds() || !_file) {
		_last_roll = t;
		_last_flush = t;
		_file.reset(new SequenceWriteFile(filename));
		return true;
	}
	return false;	
}

template <typename MUTEX>
inline void LogFile<MUTEX>::append_unlock(const char* logline, size_t len)
{
	_file->append(logline, len);
	
	if (_file->write_size() > _roll_size) {
		roll();
	} else {
		++_count;
		if (_count >= _check_size) {
		  	_count = 0;
		  	Timestamp now;
		  	if (Timespan(now.total_micro_seconds()).days() != 
                Timespan(_last_roll.total_micro_seconds()).days() ) {
				roll();
		  	} else if (now - _last_flush > _flush_step * Timestamp::ratio()) {
				_last_flush = now;
				_file->flush();
		  	}
		}
	}
}

template <typename MUTEX>
inline void LogFile<MUTEX>::flush()
{
    ScopedLock<MUTEX> lock(_mutex);
	_file->flush();
}

template <typename MUTEX>
inline std::string LogFile<MUTEX>::get_log_file_name(const Timestamp &stamp)
{
    std::string filename;
	filename.reserve(_base_name.size() + 64);
	filename = _base_name;
    
	time_t now = stamp.seconds();
	char timebuf[32];
 	struct tm tm;
	localtime_r(&now, &tm); // FIXME: localtime_r ?
	strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
	filename += timebuf;
	filename += ".log";
	return filename;
}

}
#endif
