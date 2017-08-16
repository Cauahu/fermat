#include <fermat/common/logging.h>
#include <fermat/common/log_async.h>
#include <fermat/common/cmdline.h>
#include <fermat/common/timespan.h>
#include <fermat/common/timestamp.h>
#include <fermat/common/thread.h>
#include <iostream>
#include <atomic>
#include <vector>

static int long_count = 0;
static int log_len = 128;
static int thread_number = 0;
static std::string type;
std::string long_string(512, 'x');
void runner()
{
    int cnt = 0;
    int max = long_count/thread_number;
    while (cnt < max) {
        LOG_INFO<<10<<" "
            <<200<<" "
            <<"sddf "
            <<"dfggf "
            <<long_string;
            cnt++;
    }
}
fermat::Timespan run_test()
{
    long_string.clear();
    long_string.append(log_len, 'x');
    std::vector<fermat::Thread*> ths;
    fermat::Timestamp start;
    fermat::LogOutputPtr alogger(new fermat::LogAsync("./log/async", 1024*1024*100));
    fermat::LogAsync *la = (fermat::LogAsync*)alogger.get();
    la->start();
    if(!alogger) {
        exit(-0);
    }
    fermat::Logging::set_output(alogger);
    for(int i = 0; i < thread_number; i++) {
        fermat::Thread *t = new fermat::Thread("log_test");
        t->start(std::bind(&runner));
        ths.push_back(t);
    }
    
    for(int i = 0; i < thread_number; i++) {
        fermat::Thread *t = ths[i];
        t->join();
        delete t;
    }
    fermat::Timestamp end;
    la->stop();
    return fermat::Timespan(end - start);
}

int main(int argc, char** argv)
{
    fermat::CmdParser p;
    p.add<int>("number", 'n', "log number [1, 20]", false, 10000000,fermat::range(1, 100000000));
    p.add<int>("client", 'c', "thread number [1, 20]", false, 10,fermat::range(1, 20));
    p.add<int>("loglen", 's', "log long [1, 4096]", false, 40,fermat::range(1, 4096));
    p.add<std::string>("type", 't', "log sync  type", false, "async", fermat::oneof<std::string>("async", "sync", "stdout"));
    p.parse_check(argc, argv);
    long_count = p.get<int>("number");
    thread_number = p.get<int>("client");
    type = p.get<std::string>("type");
    log_len = p.get<int>("loglen");
    fermat::Timespan span = run_test();
    std::cout<<"cost micro_seconds: "
            <<span.total_micro_seconds()
            <<std::endl;
    return 0;
}
