/*
Main Thread Id is -1772332280
Map -1772332280 : 0
Map -1772332280 : 1
Map -1772332280 : 2
Map -1772332280 : 3
Map2 -1776088264 : 0
Map -1772332280 : 4
Map2 -1776088264 : 1
Map2 -1776088264 : 2
Map2 -1776088264 : 3
Map2 -1776088264 : 4
Subs -1775944904 : 0
Subs -1775944904 : 1
Subs -1775944904 : 2
Subs -1775944904 : 3
Subs -1775944904 : 4
*/
#include "rxcpp/rx.hpp"
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %d\n",
        std::this_thread::get_id());
    //-------- We are using observe_on here
    //-------- The Map will use the main thread
    //-------- The Map2 will use the new thread 1
    //-------- Subscribed lambda will use the new thread 2
    rxcpp::observable<>::range(0, 15)
    .map([](int i) {
        printf("Map %d : %d\n", std::this_thread::get_id(), i);
        return i;
    })
    .take(5)
    .observe_on(rxcpp::synchronize_new_thread())
    .map([](int i) {
                printf("Map2 %d : %d\n", std::this_thread::get_id(), i);
                return i;
    })
    .observe_on(rxcpp::synchronize_new_thread()) //overrides the last observe_on from now on, so calls bellow
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
