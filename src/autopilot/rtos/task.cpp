#include "task.h"

Task::Task(string name, int priority, int timer, 
           func_ptr* callback, void* parameters):
    _name(name),_priority(priority),_timer(timer),
    _callback(callback),_running(false) {
    // Initialize other parameters of the constructor
    _state = UNDEFINED_STATE;
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
        _thread = make_shared<thread>(&Task::_callback_process,this);
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

void Task::_callback_process(){
    // Start the thread loop for the current task
    while (_running) {
        // Check whether the thread has been set to run a callback
        if (_callback != nullptr){
            // Call to the function directly
            (*_callback)();
        }
        else {
            // Call to the default function to override
            _default_callback();
        }
    }

}



