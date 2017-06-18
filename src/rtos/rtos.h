#ifndef RTOS_H_
#define RTOS_H_

class RTOS {
    public: 
        RTOS(int timer = 1000);
        ~ RTOS();

        bool start();
        bool stop();
        bool is_running();

    private:

        // Whether the task is running or not
        bool _running = false;
        // Time for the ticks in Milliseconds
        int _timer = 1000; 
};


#endif