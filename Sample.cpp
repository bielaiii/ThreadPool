#include<iostream>
#include <memory>
#include<thread>
#include<future>
#include<print>

using namespace std;

 
struct AAA {
    int a = 10;
};
int main()
{
    auto ft = std::make_shared<AAA>();
    cout << ft->a << endl;
    return 0;

}