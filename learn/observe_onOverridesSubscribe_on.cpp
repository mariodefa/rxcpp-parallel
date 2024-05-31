#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

/*
Main Thread Id is 188955400
Map 185199416 : 0
Map2 185199416 : 0
Map 185199416 : 1
Map2 185199416 : 1
Map 185199416 : 2
Map2 185199416 : 2
Map 185199416 : 3
Map2 185199416 : 3
Map 185199416 : 4
Map2 185199416 : 4
Subs 185342776 : 0
Subs 185342776 : 1
Subs 185342776 : 2
Subs 185342776 : 3
Subs 185342776 : 4
*/
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %d\n",
        std::this_thread::get_id());
    //-------- We are using subscribe_on here but observe_on overrides the end of the pipe
    //-------- So Map will use new thread 1
    //-------- Map2 will use new thread 1
    //-------- but subscribe will use new thread 2
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
    .observe_on(rxcpp::synchronize_new_thread())
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