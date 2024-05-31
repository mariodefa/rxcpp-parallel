/*
Main Thread Id is 263666440
Map 263666440 : 0
Map 263666440 : 1
Map 263666440 : 2
Map 263666440 : 3
Map 263666440 : 4
Map2 260053816 : 0
Subs 260053816 : 0
Map2 260053816 : 1
Subs 260053816 : 1
Map2 260053816 : 2
Subs 260053816 : 2
Map2 260053816 : 3
Subs 260053816 : 3
Map2 260053816 : 4
Subs 260053816 : 4
*/
#include "rxcpp/rx.hpp"
int main() {
    //------- Print the main thread id
    printf("Main Thread Id is %d\n",
        std::this_thread::get_id());
    //-------- We are using observe_on here
    //-------- The Map will use the main thread
    //-------- The Map2 will use the new thread
    //-------- Subscribed lambda will use the new thread
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
