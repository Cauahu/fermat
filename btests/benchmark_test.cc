#include <benchmark/benchmark.h>
#include <StringTest.h>

//args meaning 
//  iterations -----------------------
//  runs ------------------------     |
//  fuction --------------      |     | 
//  class -----          |      |     |
//            |          |      |     |  

BENCHMARK(StringTest, to_lower, 10,  100) {
    StringTest(100).to_lower();
}
