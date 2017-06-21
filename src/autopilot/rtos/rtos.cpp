#include "rtos.h"

RTOS::RTOS(uint64_t timer)
    :Task("RTOS", HIGH_PRIORITY, timer) {
    // Create a new Scheduler by default (delfault pointer)
    _stateless = true;
    _scheduler = new Scheduler(10);
}

RTOS::~RTOS(){
    // Dispose the memory allocated
    if ( _scheduler != nullptr){
        if (_scheduler->is_running()){
            _scheduler->stop();
        }
        delete _scheduler;
        _scheduler = nullptr;
    }
}

void idle_process(){
    cout << "This is the Idle Process" << endl;
}

void socket_process(){
    cout << "This is the Socket Process" << endl;
}

void tsip_process(){
    cout << "This is the TSIP Process" << endl;
}

void display_process(){
    cout << "This is the display Process" << endl;
    //cout << std::chrono::system_clock::now() << endl;
    //std::cout << "\x1B[2J\x1B[H";
    // #include <stdlib.h>
    // system("cls");
}

bool RTOS::init(){
    // Initilaize the different subsystems, drivers, etc..
    _scheduler->add(Task("idle", LOW_PRIORITY, 500, idle_process));
    // Sokets for TCPIP/COM connection
    _scheduler->add(Task("socket", HIGH_PRIORITY, 200, socket_process));
    // TSIP task, 200Hz, Non highest nor lowest.
    _scheduler->add(Task("TSIP", MID_PRIORITY, 200, tsip_process));
    // Display Task to show the time
    _scheduler->add(Task("display",LOW_PRIORITY, 1000, display_process));
 
    // Return the result of the initilization
    return true;
}

bool RTOS::start() {
    // Start the scheduler task at the same time as the current RTOS loop
    _scheduler->start();
     // Call to parent function
    return Task::start();
}


void RTOS::shutdown(){
    // Start the scheduler task at the same time as the current RTOS loop
    _scheduler->stop();
    // finally stop the main thread
    stop();
}

void RTOS::_default_callback(){
    // Main Process for the OS

    cout << "RTOS Working" << endl;
   
}




