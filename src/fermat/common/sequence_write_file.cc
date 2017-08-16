#include <fermat/common/sequence_write_file.h>
#include <cassert>
namespace fermat {

SequenceWriteFile::SequenceWriteFile(const char* fileName) 
    :_file_name(fileName),
     _fp(::fopen(fileName, "ae")),
     _write_size(0)
{
    assert(_fp);
	::setbuffer(_fp, _buffer, sizeof _buffer);
}

SequenceWriteFile::SequenceWriteFile(const std::string& fileName)
    :_file_name(fileName),
     _fp(::fopen(_file_name.c_str(), "ae")),
     _write_size(0)
{
    assert(_fp);
	::setbuffer(_fp, _buffer, sizeof _buffer);
}

SequenceWriteFile::~SequenceWriteFile()
{
    if(_fp) {
	    ::fclose(_fp);
        _fp = NULL;
	}
}

void SequenceWriteFile::flush()
{
    if(_fp) {
		::fflush(_fp);
	}
}

    
bool SequenceWriteFile::append(const char* content, const size_t len)
{
    size_t n = unlock_write(content, len);
	size_t remain = len -n;
	while (remain > 0) {
		n = unlock_write(content+len-remain,remain);
		if (n == 0) {
			int err = ferror(_fp);
			if(err) {
				fprintf(stderr,  "SequenceWriteFile::append() failed %d\n", err);
				return false;
			}
		}
		remain = len -n;
	}
	_write_size += len;
	return true;    
}
    
size_t SequenceWriteFile::write_size()
{
    return _write_size;
}

size_t SequenceWriteFile::unlock_write(const char* content, const size_t len)
{
#if !(defined(__APPLE__) && defined(__MACH__))
    return ::fwrite_unlocked(content, 1, len, _fp);
#else 
    return ::fwrite(content, 1, len, _fp);
#endif
}

}