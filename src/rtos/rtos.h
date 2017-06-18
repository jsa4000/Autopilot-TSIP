#ifndef _RTOS_H_
#define _RTOS_H_

#include "task.h"

class RTOS : public Task {
    public: 
        RTOS(int timer = 1000);
        ~ RTOS();

    private:
        //Function to overwrite
        void _process();
       
};

#endif //_RTOS_H_