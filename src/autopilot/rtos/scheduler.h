#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "task.h"

class Scheduler : public Task {
    public: 
        Scheduler(uint64_t timer = 10);
        ~ Scheduler();

        void add(Task task);
      
    private:
    
        //Function to overwrite
        void _default_callback();
        Task *_find_next();

        // Current Task
        Task *_current = nullptr;

        // Priority queues
        queue<Task> _high_priority;
        queue<Task> _mid_priority;
        queue<Task> _low_priority;
      
};

#endif //_SCHEDULER_H_