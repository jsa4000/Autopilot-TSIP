#ifndef _RTOS_H_
#define _RTOS_H_

#include "task.h"
#include "scheduler.h"
#include "queue.h"

template class Queue<int>;

class RTOS : public Task {
    public: 
        RTOS(uint64_t timer = 1000);
        ~ RTOS();

        bool    init();
        void    shutdown();
        bool    start();

    private:
    
        //Function to overwrite
        void _default_callback();
        // Scheduler for Task Management
       shared_ptr<Scheduler> _scheduler = nullptr;        
       
};

#endif //_RTOS_H_