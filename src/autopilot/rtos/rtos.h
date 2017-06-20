#ifndef _RTOS_H_
#define _RTOS_H_

#include "task.h"

class RTOS : public Task {
    public: 
        RTOS(uint64_t timer = 1000);
        ~ RTOS();

        bool init();
        void shutdown();

    private:
    
        //Function to overwrite
        void _default_callback();
       
};

#endif //_RTOS_H_