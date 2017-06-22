#ifndef _COM_H_
#define _COM_H_


#include <cstdint>

/**
* \brief Read received raw data from COM port.
*
* This is a non-blocking read function. This means that only available received
* data will be served. User may decide to call this function within a loop
* until the desired amount of data is received.
*
* \param buf Pointer to destination buffer where to copy received data.
* \param count Maximum number of bytes to be read.
* \return Number of read bytes. This value will always be <= count.
*/
int32_t uavnComRead(uint8_t * const buffer, const uint32_t count);

#endif // _COM_H_