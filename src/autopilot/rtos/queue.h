#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "headers.h"

template <typename T> 
class Queue {
    public: 
        Queue(){}
        ~ Queue(){}
      
        void push(T item){
            // Critical section
            _lock.lock();
                // Add the element to the final
                _queue.push(item);
            // End critical section
            _lock.unlock();
        }

        T pop(){
            // Critical section
            _lock.lock();
                // First get the front element
                T _result = _queue.front();
                // Remove current element from the queue
                _queue.pop();
            // End critical section
            _lock.unlock();
            return _result;
        }

        uint32_t size(){
            // Critical Section
            _lock.lock();
            //lock_guard<mutex> guard(_lock); // If const
                // Get the current size of the queue
                uint32_t _result= _queue.size();
            _lock.unlock();
            // End critical section
            return _result;
        }

         bool empty(){
            // Critical Section
            _lock.lock();
            //lock_guard<mutex> guard(_lock); // If const
                // Get the current size of the queue
                bool _result= _queue.empty();
            _lock.unlock();
            // End critical section
            return _result;
        }
   
    private:

        mutex _lock;
        queue<T> _queue;
      
};

#endif //_QUEUE_H_