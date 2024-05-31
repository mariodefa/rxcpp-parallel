#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>

/* promt:
-------------------------------------------------
140026930015032<==>140026929871672 tick: 1
140026930015032<==>140026929871672 tick: 2
140026930015032<==>140026929871672 tick: 3
140026930015032<==>140026929441592 -- 140026933771016->,140026929728312 call1 result,140026929584952 call2 result,140026929441592 call3 result
140026930015032<==>140026929871672 tick: 4
140026930015032<==>140026929871672 tick: 5
140026930015032<==>140026929871672 tick: 6
140026930015032 Tick Completed!
-------------------------------------------------
ids:
140026930015032 = evLoopThread
140026929728312 = t1
140026929584952 = t2
140026929441592 = t3
140026929871672 = t4 for ticks
140026933771016 = merge allCalls t?
*/
// Helper function to get thread ID as string
std::string get_pid1() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

int main() {
    auto evLoopThread = rxcpp::observe_on_event_loop();

    auto delay1s = rxcpp::observable<>::just(" call1 result")
        .subscribe_on(evLoopThread)
        .map([](std::string e){std::this_thread::yield();return get_pid1()+e;})//do delay on new thread 1
        .delay(std::chrono::seconds(1));

    auto delay2s = rxcpp::observable<>::just(" call2 result")
        .subscribe_on(evLoopThread)
        .map([](std::string e){std::this_thread::yield();return get_pid1()+e;})//do delay on new thread 2
        .delay(std::chrono::seconds(2));

    auto delay3s = rxcpp::observable<>::just(" call3 result")
        .subscribe_on(evLoopThread)
        .map([](std::string e){std::this_thread::yield();return get_pid1()+e;})//do delay on new thread 3
        .delay(std::chrono::seconds(3));

    //join observables
    std::vector<rxcpp::observable<std::string>> allCalls = {delay1s, delay2s, delay3s};
    rxcpp::observable<rxcpp::observable<std::string>> allCallsWrapper = rxcpp::observable<>::iterate(allCalls);
    
    rxcpp::observable<std::string> resultChannel = allCallsWrapper   
        .merge()//run delays/calls in parallel
        .scan(std::string(get_pid1()+"->"), [](std::string acc, std::string cur) { return acc + "," + cur; }) //auto extra thread runs this
        .buffer(3)//collect them altogether, output will looks like ["1","12","123"]
        .map([](const std::vector<std::string>& results) {
            return get_pid1()+" -- "+results.back();//new thread 3 run this, because it is the last one arriving here?
        });//take the latest as result, so following previous example it takes "123" as result  

    //using starttick 500 ms to give advantage to resultChannel, so clockChannel starts 500ms later than resultChannel
    rxcpp::observable<int> startTick = rxcpp::observable<>::just(0)
        .subscribe_on(evLoopThread)
        .map([](auto e){std::this_thread::yield();return e;})//do delay on new thread 4
        .delay(std::chrono::milliseconds(500));
    rxcpp::observable<int> oneSecTicks = rxcpp::observable<>::interval(std::chrono::seconds(1))
        .filter([](int i){return i!=1;})//discard inmidiate first tick 1
        .map([](int t) { return t-1; });
    rxcpp::observable<std::string> clockChannel = startTick.concat(oneSecTicks)
        .filter([](int i){return i!=0;})//discard startTick fake tick 0
        .take(6)        
        .map([](int value) { return std::string(get_pid1()+" tick: "+std::to_string(value)); });        

    //clockChannel starts 500ms later than resultChannel
    rxcpp::observable<>::from(clockChannel, resultChannel)          
        .merge()//run clock and allCalls in parallel
        .observe_on(evLoopThread)
        .as_blocking()
        .subscribe(
            [](const std::string& value) { std::cout << get_pid1()+ "<==>"+ value << std::endl; },
            []() { std::cout << get_pid1()+" Tick Completed!" << std::endl; }
        );

    return 0;
}
