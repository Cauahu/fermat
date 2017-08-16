#ifndef FERMAT_COMMON_SEQUENCE_WRITE_FILE_H_
#define FERMAT_COMMON_SEQUENCE_WRITE_FILE_H_
#include <cstdint>
#include <string>
#include <cstdio>
#include <cstddef>
#include <cstdlib>

namespace fermat {

class SequenceWriteFile {
public:
    SequenceWriteFile(const char* fileName);
    SequenceWriteFile(const std::string& fileName);
    ~SequenceWriteFile();

    static const size_t kTempBuffSize = 64 * 1024;

    void flush();
    
    bool append(const char* content, const size_t len);
    
    size_t write_size();
private:
    size_t unlock_write(const char* content, const size_t len);
private:
    std::string  _file_name;
    FILE        *_fp;
    size_t       _write_size;
    char         _buffer[kTempBuffSize];

};

}
#endif
