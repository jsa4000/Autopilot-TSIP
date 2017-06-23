#include "scheduler.h"

Scheduler::Scheduler(uint64_t timer)
    :Task("Scheduler", HIGH_PRIORITY, timer) {
     // Set current task as Stateless
    _stateless = true;
}

Scheduler::~Scheduler(){
    // Clean the Momery allocated
 
}

void Scheduler::add(shared_ptr<Task> task){
    switch (task->get_priority()) {
        case HIGH_PRIORITY:
            // Add the task of highest priority
            _high_priority.push(task);
            break;
        case MID_PRIORITY:
             // Add the task of mid priority
            _mid_priority.push(task);
            break;
         case LOW_PRIORITY:
             // Add the task of lowest priority
            _low_priority.push(task);
            break;
    }
}

shared_ptr<Task> Scheduler::_find_next(){
    shared_ptr<Task> _result = nullptr;
    // This function will iterate over all the list to get the next
    // task to activate. The states will be READY_STATE or NO_STATE
    for (int i=0;i<3;i++){
        queue<shared_ptr<Task>> *current_queue;
        switch (i) {
            case 0:
                current_queue = &_high_priority;
                break;
            case 1:
                current_queue = &_mid_priority;
                break;
            case 2:
                current_queue = &_low_priority;
                break;
        }
        // Check the current size of the queue
        if (current_queue->size()>0) {
            // For each element get the next Task to run
            shared_ptr<Task> first_task = current_queue->front();
            do {
                shared_ptr<Task> current_task = current_queue->front();
                // Extract the current Task from the queue
                current_queue->pop();
                // Put into the queue again (At the end)
                current_queue->push(current_task);
                // Check the current state of the task
                if (current_task->get_state()==NO_STATE ||
                    current_task->get_state()==READY_STATE){
                    _result = current_task;
                    break;
                }
            }while (current_queue->front()->get_name()!=first_task->get_name());
            // Check if we have found a task to run
            if (_result)
                break;
        }
       
    }
    // Return the current result
    return _result;
}

void Scheduler::_default_callback(){
    //Main Process for the Scheduler
    if (_current && _current->get_state()==RUN_STATE)
         return;

    //Get the next task to process
    _current=_find_next();
    //Launch the Task with highest priority in order
    if (_current){
        if (_current->get_state()==NO_STATE) {
            // Check if the task has been started from first time
            _current->start();
        } 
        // Change it's current State to RUNNING
        _current->set_state(RUN_STATE);
    }
}




