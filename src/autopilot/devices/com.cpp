#include "com.h"
#include "tsip.h"
#include <string>
#include <stdlib.h>  
#include <cstring>
#include <time.h>     

const std::string messages[] = {"R 7D 01 Position Fix Rate Options Report",
                            "1 byte opt1flags",
                            "2 byte opt2flags",
                            "R 7D 03 Maximum Measurement Age Report",
                            "1 integer max_age 1-5000 ms : Allow meas used in pos to be at most this old",
                            "R 7D 05 CTS to Transmit Delay Report",
                            "1 integer cts_delay 0-32000 ms",
                            "R 7D 06 RTS Trailing Edge Delay Report",
                            "1 integer rts_delay 0-32000 ms",
                            "R 7D 09 Time-Based Message Interval Report",
                            "1 byte port ",
                            "2 byte protocol 0=TSIP/1=NMEA",
                            "3 byte interval interval code",
                            "4-11 byte reserved",
                            "12 integer checksum",
                            "R 7D 7F Fast Rate Option Not Installed Notification",
                            "R 82 - Differential Position Fix Mode Report",
                            "0 byte mode flags",
                            "1 byte rtcm_vers flags",
                            "2 integer refstatn Reference station ID (-1=accept any)"};

int32_t uavnComRead(uint8_t * const buffer, const uint32_t count){
    int32_t byte_count = 0;
    // Initialize random seed:
    srand (time(nullptr));
    // Generates random number between 1 and total messages defined
    int total_messages = rand() % _countof(messages) + 1;
    // Copy messages in TSIP format into the buffer 
    for (int i=0; i < total_messages; i++){
        // And the Start of the messages
        buffer[byte_count++] = DLE;
        // Check if the buffer is full
        if (messages[i].size() + byte_count > count) break;
        // Add current message to the buffer
        for (int j=0; j < messages[i].size(); j++){
            buffer[byte_count++] = messages[i][j];
        }
        // Add the DLE and ETX
        buffer[byte_count++] = DLE;
        buffer[byte_count++] = ETX;
    }
    // Return the nubmber of bytes copied into the buffer
    return byte_count;
}