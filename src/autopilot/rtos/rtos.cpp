#include "rtos.h"

RTOS::RTOS(uint64_t timer)
    :Task("RTOS", HIGH_PRIORITY, timer) {

}

RTOS::~RTOS(){
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
    _tasks.push_back(Task("idle", LOW_PRIORITY, 500, idle_process));
    // Sokets for TCPIP/COM connection
    _tasks.push_back(Task("socket", HIGH_PRIORITY, 200, socket_process));
    // TSIP task, 200Hz, Non highest nor lowest.
    _tasks.push_back(Task("TSIP", MID_PRIORITY, 200, tsip_process));
    // Display Task to show the time
    _tasks.push_back(Task("display",LOW_PRIORITY, 1000, display_process));
 
    // Init Scheduler Task Manager to start the task
    for ( auto &task : _tasks ) {
        task.start();
    }
    // Return the result of the initilization
    return true;
}

void RTOS::shutdown(){

    // finally stop the main thread
    stop();
}

void RTOS::_default_callback(){
    // Main Process for the OS

    cout << "RTOS Working" << endl;
   
}




