#include <stdint.h>

/**
 * \brief CRC calculation for TSIP packets.
 *
 * \param buf   Pointer to buffer containing data to process.
 * \param start Offset value of first data byte (within buf) to be processed.
 * \param end   Offset value of last data byte (within buf) to be processed.
 * \return      Calculated CRC value.
 */ 
uint32_t crc32(uint8_t *buf, uint32_t start, uint32_t end);


int four();