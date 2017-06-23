#include "tsip.h"

#include <iostream>
#include <string>


void ParseTsipData(const uint8_t * const buffer, 
                    const int32_t numberOfBytes){
    // You can assume that this function consume the data 
    //performing the required autopilot actions. You do not 
    //need to program this function.
    std::string tsip_message((char *)buffer,numberOfBytes ); 
    // Cout the current message received
    std::cout << "TSIP Message Received: " << tsip_message << std::endl;
    
}