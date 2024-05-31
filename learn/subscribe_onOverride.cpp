#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

//cannot override
/*
Main Thread Id is 2125036296
Map 2121423672 : 0
Subs 2121423672 : 0
Map 2121423672 : 1
Subs 2121423672 : 1
Map 2121423672 : 2
Subs 2121423672 : 2
Map 2121423672 : 3
Subs 2121423672 : 3
Map 2121423672 : 4
Subs 2121423672 : 4
*/
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %d\n",
        std::this_thread::get_id());
    //-------- We are using subscribe_on here
    //-------- The Map and subscribed lambda will 
    //--------- use the secondary thread
    rxcpp::observable<>::range(0, 15)
    .map([](int i) {
        printf("Map %d : %d\n", std::this_thread::get_id(), i);
        return i;
    })
    .take(5)
    .subscribe_on(rxcpp::synchronize_new_thread())
    .map([](int i) {
        printf("Map2 %d : %d\n", std::this_thread::get_id(), i);
        return i;
    })
    .subscribe_on(rxcpp::observe_on_event_loop())
    .subscribe([&](int i) {
        printf("Subs %d : %d\n", std::this_thread::get_id(), i);
    });

    //----------- Wait for Two Seconds
    rxcpp::observable<>::timer(
        std::chrono::milliseconds(2000)
    )
    .subscribe([&](long) {});

    return 0;
}