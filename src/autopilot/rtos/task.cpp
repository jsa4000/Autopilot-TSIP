#include "task.h"

Task::Task(int timer):
    _timer(timer),_running(false) {
}

Task::~Task(){
    if (_thread != NULL){
        _thread == NULL;
    }
}

bool Task::start(){
    // Check if the thread already exist
    if (_thread == NULL) {
        // Set running as true
        _running = true;
        // Create the new thread
        _thread = make_shared<thread>(&Task::_process,this);
        // return true
        return true;
    }
    // Proccess still running
    return false;
}
bool Task::stop(){
    if (_running) {
        // Set running as False to Stop the thread
        _running = false;
        // Pause until the thread ends
        _thread->join();
    }
    return true;
}

bool Task::is_running(){
    return _running;
}




