#ifndef _TASK_H_
#define _TASK_H_

#include "headers.h"

// Finde different Priorities for the Tasks
#define HIGH_PRIORITY 0x01
#define MID_PRIORITY  0x02
#define LOW_PRIORITY  0x04

// Define different States for the Tasks
#define READY_STATE      0x01
#define WAITING_STATE    0x02
#define RUNNING_STATE    0x04
#define NO_STATE         0x08

// Define the function type for the callback
// https://oopscenities.net/2012/02/24/c11-stdfunction-and-stdbind/
//typedef void (*func_ptr)(void);

class Task {

    public: 
         Task(string name, uint8_t priority=MID_PRIORITY, uint64_t timer=1000,
            function<void(Task*)> callback=nullptr, void* parameters=nullptr);
        ~ Task();

        bool    start();

        bool    stop();
        
        bool    is_running();

        void    set_state(uint8_t state);
        uint8_t get_state() const;

        uint8_t get_priority()  const;
        string  get_name()  const;

        void*   get_parameters();

        void    sleep(int milliseconds);

        auto    get_current_time();

    protected:
        
        // Name of the task
        string _name;
        // Priority of the Task
        uint8_t _priority = LOW_PRIORITY;
        // Time for the ticks in Milliseconds
        uint64_t _timer; 
        // Callback function to call
        function<void(Task*)> _callback = nullptr;
        // Parameters to send to the callback function
        void* _parameters = nullptr;

        // Default State (undefined)
        int _state = NO_STATE;

        // Main loop function for the task
        void _callback_process(); 
        // Main loop function for the task
        virtual void _default_callback(void){} 
        // Thread and mutex for the Task
        shared_ptr<thread> _thread = nullptr;
        // Whether the task is running or not
        bool _running = false;
        // Stateless task
        bool _stateless = false;

};


#endif // _TASK_H_