#ifndef _TASK_H_
#define _TASK_H_

//http://www.cplusplus.com/reference/mutex/lock/
#include <thread>
using namespace std;

class Task {
    public: 
        Task(int timer = 1000);
        ~ Task();

        bool start();
        bool stop();
        bool is_running();

    protected:

        //Function to overwrite
        virtual void _process() {}
        // Whether the task is running or not
        bool _running = false;
        // Time for the ticks in Milliseconds
        int _timer; 
        // Thread and mutex
        shared_ptr<thread> _thread = NULL;
};


#endif // _TASK_H_