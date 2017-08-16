#include <string.h>
#include <fermat/common/string.h>

class StringTest {
public:
    StringTest(std::size_t times):_times(times) {
       
    }

    void to_lower() {
        std::string str("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        while(_times--) {
            fermat::to_lower<std::string>(str);
        } 
    }

private:
    std::size_t _times;
};
