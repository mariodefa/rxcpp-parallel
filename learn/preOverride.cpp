#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

/*
Main Thread Id is 1953528584
Map1 1949772600 : 0
Map1 1949772600 : 1
Map1 1949772600 : 2
Map2 1949629240 : 0
Map3 1949629240 : 0
Map1 1949772600 : 3
Map1 1949772600 : 4
Map1 1949772600 : 5
Map1 1949772600 : 6
Map1 1949772600 : 7
Map1 1949772600 : 8
Map1 1949772600 : 9
Subs 1949915960 : 0
Map1 1949772600 : 10
Map1 1949772600 : 11
Map1 1949772600 : 12
Map1 1949772600 : 13
Map1 1949772600 : 14
Map1 1949772600 : 15
Map2 1949629240 : 1
Map3 1949629240 : 1
Map2 1949629240 : 2
Map3 1949629240 : 2
Map2 1949629240 : 3
Map3 1949629240 : 3
Subs 1949915960 : 1
Subs 1949915960 : 2
Subs 1949915960 : 3
Map2 1949629240 : 4
Map3 1949629240 : 4
Subs 1949915960 : 4
*/
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %d\n",
        std::this_thread::get_id());
    //-------- subscribe_on affects Map1, so it will run in new thread 1
    //-------- first observe_on affects Map2, so it will run in new thread 2 and 3
    //--------- second observe_on affects subscribe, so it will run in new thread 4
    rxcpp::observable<>::range(0, 15)
    .map([](int i) {
        printf("Map1 %d : %d\n", std::this_thread::get_id(), i);
        return i;
    })
    .observe_on(rxcpp::synchronize_new_thread())
    .map([](int i) {
        printf("Map2 %d : %d\n", std::this_thread::get_id(), i);
        return i;
    })
    .take(5)
    .subscribe_on(rxcpp::synchronize_new_thread())
    .map([](int i) {
        printf("Map3 %d : %d\n", std::this_thread::get_id(), i);
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