#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "task.h"

class Scheduler : public Task {
    public: 
        Scheduler(uint64_t timer = 10);
        ~ Scheduler();

        void add(shared_ptr<Task> task);
      
    private:
    
        //Function to overwrite
        void _default_callback();
        shared_ptr<Task> _find_next();

        // Current Task
        shared_ptr<Task> _current = nullptr;

        // Priority queues
        queue<shared_ptr<Task>> _high_priority;
        queue<shared_ptr<Task>> _mid_priority;
        queue<shared_ptr<Task>> _low_priority;
      
};

#endif //_SCHEDULER_H_