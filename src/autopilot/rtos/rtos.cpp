#include "rtos.h"

RTOS::RTOS(uint64_t timer)
    :Task("RTOS", HIGH_PRIORITY,timer) {

}

RTOS::~RTOS(){
}

bool RTOS::init(){
    return true;
}

void RTOS::shutdown(){

    // finally stop the main thread
    stop();
}

void RTOS::_default_callback(){
            
   
         cout << "HOLA" << endl;
   
}




