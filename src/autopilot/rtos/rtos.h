#ifndef _RTOS_H_
#define _RTOS_H_

#include "task.h"

class RTOS : public Task {
    public: 
        RTOS(uint64_t timer = 1000);
        ~ RTOS();

        bool init();
        bool init2();
        void shutdown();

                // Task
        vector<Task> _tasks;

    private:
    
        //Function to overwrite
        void _default_callback();

        
       
};

#endif //_RTOS_H_