#include "pools.hpp"
#include <print>
#include <vector>
using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    auto pool = Compact::ThreadPools(10);
    auto ft = pool.enqueue(add, 1,2);
    println("{}", ft.get());
    auto sec = pool.enqueue([](float f, double d) {return f * 3.14 + d * 200;}, 4.8f, 23.0);
    println("{:.2f}", sec.get());
    return 0;
}
