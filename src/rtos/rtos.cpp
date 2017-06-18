#include <iostream>
#include <thread>         
#include <chrono>     
#include "rtos.h"

RTOS::RTOS(int timer)
    :Task(timer) {

}

RTOS::~RTOS(){
}

void RTOS::_process(){

    std::cout << "countdown:\n";
    for (int i=10; i>0; --i) {
        std::cout << i << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    std::cout << "Lift off!\n";
}




