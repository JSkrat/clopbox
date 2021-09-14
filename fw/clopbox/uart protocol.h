/*
 * uart_protocol.h
 *
 * Created: 22.05.2021 23:18:45
 *  Author: Mintytail
 */ 


#ifndef UART_PROTOCOL_H_
#define UART_PROTOCOL_H_

#include <stdbool.h>

typedef enum {
	eucOk = 0,
	
	eucGeneralFail = 0x80,
	eucBadVersion = 0x90,
	eucBadCommand = 0x91,
	eucMemoryError = 0x92,
	eucArgumentValidationError = 0x93,
	eucNotImplemented = 0x94,
} eUARTResponseCodes;

void UARTBeginTransaction();
bool UARTProcessNextByte(unsigned char b, uint8_t *respBuffer, uint8_t *respSize);



#endif /* UART_PROTOCOL_H_ */