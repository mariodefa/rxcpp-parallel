#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

std::string getThreadId() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}
/*
Main Thread Id is 140479131986696
Map5 140479131986696
Map5 140479131986696
Map2 140479128087352 : Map1 140479128087352 call1 result  //new thread 1
Map3 140479127943992 :  call2 result  //new thread 2
Map5 140479131986696
Map4 140479127800632 :  call3 result  //new thread 3
Map6 140479128087352 : Map1 140479128087352 call1 result
Subs 140479128230712 : Map1 140479128087352 call1 result
Map6 140479127943992 :  call2 result
Subs 140479128230712 :  call2 result  //event_loop thread
Map6 140479127800632 :  call3 result
Subs 140479128230712 :  call3 result 
*/
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %s\n",
        getThreadId().c_str());
    rxcpp::observable<std::string> delay1s = rxcpp::observable<>::just(std::string(" call1 result"))//in main thread
        .map([](auto i) {
            return std::string("Map1 ")+getThreadId()+i;//in new thread 1
        })
        .subscribe_on(rxcpp::observe_on_event_loop())
        .map([](auto i) {
            std::this_thread::yield();
            printf("Map2 %s : %s\n", getThreadId().c_str(), i.c_str());//in new thread 1
            return i;
        })
        .delay(std::chrono::seconds(1))    
        .take(1);

    rxcpp::observable<std::string> delay2s = rxcpp::observable<>::just(std::string(" call2 result"))//in main thread
        .subscribe_on(rxcpp::observe_on_event_loop())
        .map([](auto i) {
            std::this_thread::yield();
            printf("Map3 %s : %s\n", getThreadId().c_str(), i.c_str());//in new thread 2
            return i;
        })
        .delay(std::chrono::seconds(2))        
        .take(1);

    rxcpp::observable<std::string> delay3s = rxcpp::observable<>::just(std::string(" call3 result"))//in main thread
        .subscribe_on(rxcpp::observe_on_event_loop())
        .map([](auto i) {
            std::this_thread::yield();
            printf("Map4 %s : %s\n", getThreadId().c_str(), i.c_str());//in new thread 3
            return i;
        })
        .delay(std::chrono::seconds(3))        
        .take(1);

    std::vector<rxcpp::observable<std::string>> allCalls = {delay1s, delay2s, delay3s};
    rxcpp::observable<rxcpp::observable<std::string>> allCallsWrapper = rxcpp::observable<>::iterate(allCalls);

    allCallsWrapper
        .map([](auto i) {//in main thread
            printf("Map5 %s\n", getThreadId().c_str());
            return i;
        })
        .merge()      
        .take(3)        
        .map([](auto i) {//in each new thread, so new thread 1, 2 and 3
            printf("Map6 %s : %s\n", getThreadId().c_str(), i.c_str());
            return i;
        })
        .observe_on(rxcpp::observe_on_event_loop()) 
        .as_blocking()
        .subscribe([&](std::string value) {//in event_loop thread
            printf("Subs %s : %s\n", getThreadId().c_str(), value.c_str());
        });

    return 0;
}