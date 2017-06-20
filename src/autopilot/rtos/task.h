#ifndef _TASK_H_
#define _TASK_H_

#include "headers.h"

// Finde different Priorities for the Tasks
#define HIGH_PRIORITY 0x01
#define MID_PRIORITY  0x02
#define LOW_PRIORITY  0x04

// Define different States for the Tasks
#define READY_STATE      0x01
#define WAIT_STATE       0x02
#define RUN_STATE        0x04
#define UNDEFINED_STATE  0x08

// Define the function type for the callback
typedef void (*func_ptr)(void);

class Task {

    public: 
        Task(string name, int priority=MID_PRIORITY, int timer=1000,
             func_ptr* callback=nullptr, void* parameters=nullptr);
        ~ Task();

        bool start();

        bool stop();
        
        bool is_running();

    protected:
        
        // Name of the task
        string _name;
        // Priority of the Task
        int _priority = LOW_PRIORITY;
        // Time for the ticks in Milliseconds
        int _timer; 
        // Callback function to call
        func_ptr* _callback = nullptr;
        // Parameters to send to the callback function
        void* _parameters = nullptr;

        // Default State (undefined)
        int _state = UNDEFINED_STATE;

        // Main loop function for the task
        void _callback_process(); 
        // Main loop function for the task
        virtual void _default_callback(void){} 
        // Thread and mutex for the Task
        shared_ptr<thread> _thread = nullptr;
        // Whether the task is running or not
        bool _running = false;

};


#endif // _TASK_H_