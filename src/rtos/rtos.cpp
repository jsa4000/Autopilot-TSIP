#include "rtos.h"

RTOS::RTOS(int timer):
    _timer(timer),_running(false) {

}

RTOS::~RTOS(){
}

bool RTOS::start(){
    // Set running as true
    _running = true;
}
bool RTOS::stop(){
    // Set running as False to Stop the thread
    _running = false;
}

bool RTOS::is_running(){
    return _running;
}




