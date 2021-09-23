/*
 * uart_functions.h
 *
 * Created: 23.05.2021 00:23:35
 *  Author: Mintytail
 */ 


#ifndef UART_FUNCTIONS_H_
#define UART_FUNCTIONS_H_

typedef uint8_t (*fUARTFunction)(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize);

typedef enum {
	ufEcho = 0,
	ufVersion = 1,
	
	ufResetOutputs = 0x10,
	ufSetOutput = 0x12,
	ufSetOutputs = 0x14,
	ufGetOutputs = 0x15,
	ufSetOutputWithTimeout = 0x16,
	
} eUARTFunction;

typedef struct {
	eUARTFunction function;
	fUARTFunction callback;
} tUARTCommandItem;

#define UART_FUNCTIONS_NUMBER 7
extern const tUARTCommandItem UARTFunctions[UART_FUNCTIONS_NUMBER];

#endif /* UART_FUNCTIONS_H_ */