#include "com.h"

/*
 Check no exception reading Serial Port
*/
bool test_uavnComRead(){
    try {
        //Create an empty buffer
        buffer[512];
        //Call to the function
        uavnComRead(buffer, sizeof buffer);
        // Check the number of bytes
    } catch (std::ex const std::exception& e){
        // Test failed to return the TSIP data
        return false;
    }
}

/*
 Check if the Bytes read from Serial Port is > 0
*/
bool test_bytes_read_uavnComRead(){
    try {
        //Create an empty buffer
        buffer[512];
        //Call to the function
        int32_t count = uavnComRead(buffer, sizeof buffer);
        // Check the number of bytes read from COM
        if (count <= 0) return false;
        return true;
    } catch (std::ex const std::exception& e){
        // Test failed to return the TSIP data
        return false;
    }
}