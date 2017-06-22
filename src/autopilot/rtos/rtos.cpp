#include "rtos.h"

RTOS::RTOS(uint64_t timer)
    :Task("RTOS", HIGH_PRIORITY, timer) {
    // Create a new Scheduler by default (delfault pointer)
    _stateless = true;
    _scheduler = make_shared<Scheduler>(10);
}

RTOS::~RTOS(){
    // Dispose the memory allocated
    if (_scheduler && _scheduler->is_running()){
        _scheduler->stop();
        _scheduler = nullptr;
    }
}

void idle_process(Task* task){
    cout << "This is the Idle Process" << endl;
}

void socket_process(Task* task){
    cout << "This is the Socket Process" << endl;

    Queue<string> *packet_queue = (Queue<string>*) task->get_parameters();

    packet_queue->push("Manolo");
    packet_queue->push("Javier");
    packet_queue->push("Ana");
    packet_queue->push("Tomás");
}

void tsip_process(Task* task){
    cout << "This is the TSIP Process" << endl;
    
    Queue<string> *packet_queue = (Queue<string>*) task->get_parameters();

    while (!packet_queue->empty()) {
        cout << packet_queue->pop() << endl;
    }
    
}

void display_process(Task* task){
    cout << "This is the display Process" << endl;
    //cout << std::chrono::system_clock::now() << endl;
    //std::cout << "\x1B[2J\x1B[H";
    // #include <stdlib.h>
    // system("cls");
}

bool RTOS::init(){
    // Create shared queue to be passed between tasks socket and tsip
    //shared_ptr<Queue<string>> packet_queue = make_shared<Queue<string>>();
    Queue<string> *packet_queue = new Queue<string>();
    // Initilaize the different subsystems, drivers, etc..
    _scheduler->add(make_shared<Task>("idle", LOW_PRIORITY, 500, idle_process));
    // Sokets for TCPIP/COM connection
    _scheduler->add(make_shared<Task>("socket", HIGH_PRIORITY, 400, socket_process, packet_queue));
    // TSIP task, 200Hz, Non highest nor lowest.
    _scheduler->add(make_shared<Task>("TSIP", MID_PRIORITY, 200, tsip_process, packet_queue));
    // Display Task to show the time
    _scheduler->add(make_shared<Task>("display",LOW_PRIORITY, 1000, display_process));
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




