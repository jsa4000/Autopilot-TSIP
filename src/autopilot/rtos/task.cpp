#include "task.h"

Task::Task(string name, uint8_t priority, uint64_t timer,
            function<void()> callback, void* parameters):
    _name(name),_priority(priority),_timer(timer),
    _callback(callback),_parameters(parameters),_running(false) {
    // Initialize other parameters of the constructor
    _state = UNDEFINED_STATE;
}

Task::~Task(){
    if (_thread != NULL){
        _thread == NULL;
    }
}

void Task::sleep(int milliseconds){
     std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

bool Task::start(){
    // Check if the thread already exist
    if (_thread == NULL) {
        // Set running as true
        _running = true;
        // Create the new thread
        _thread = make_shared<thread>(&Task::_callback_process,this);
        // Detach the thread from the current context
        _thread->detach();
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

void Task::set_state(uint8_t state){
    _state = state;
}

uint8_t Task::get_state(){
    return _state;
}

auto Task::get_current_time(){
    return std::chrono::system_clock::now();
}

void Task::_callback_process(){
    // Get the time-ticks for the current task
    std::chrono::system_clock::time_point start_time, end_time;
    uint64_t timediff;
    _state = READY_STATE;
    // Start the thread loop for the current task
    while (_running) {
        // Check if the task must be launched
        if (_state != RUN_STATE) {
            //Wait until run state is on
            sleep(10);
            continue;
        }
        // Get the start time
        start_time = get_current_time();
        // Check whether the thread has been set to run a callback
        if (_callback != nullptr){
            // Call to the function directly
            _callback();
        }
        else {
            // Call to the default function to override
            _default_callback();
        }
        // Get the end time
        end_time = get_current_time();
        // Check until the next tick for perform the task
        timediff = std::chrono::duration_cast<std::chrono::milliseconds>
              (end_time - start_time).count();
        // Check if the Task need sleep some time before the next iteration
        if (timediff < _timer)
            sleep(_timer - timediff);
        // Set the current Task as READY_STATE
        _state = READY_STATE;
    }

}



