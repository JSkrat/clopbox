/*
 * uart_protocol.c
 *
 * Created: 22.05.2021 23:21:06
 *  Author: Mintytail
 */ 

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stddef.h> // for NULL
#include "uart protocol.h"
#include "uart functions.h"

#define PROTO_VERSION 0

typedef enum {
	usError,
	usProtoVer,
	usCommand,
	usPDataLength,
	usPData,
	usEnd,
} usState;

#define MAX_DATA_LENGTH 16
static usState state;
static uint8_t position = 0;
static uint8_t command, payloadSize, payload[MAX_DATA_LENGTH];

void generateErrorResponse(const eUARTResponseCodes code, uint8_t *response, uint8_t *length) {
	response[0] = PROTO_VERSION;
	response[1] = 0xFF; // command
	response[2] = code;
	response[3] = 0; // payload size
	*length = 4;
}

void generateResponse(uint8_t *response, uint8_t *length) {
	response[0] = PROTO_VERSION;
	response[1] = command;
	for (uint8_t i = 0; i < UART_FUNCTIONS_NUMBER; i++) {
		eUARTFunction iCommand = pgm_read_byte((const uint8_t*) &(UARTFunctions[i].function));
		if (command == iCommand) {
			fUARTFunction iFunction = (fUARTFunction) pgm_read_ptr(&(UARTFunctions[i].callback));
			if (NULL == iFunction) {
				response[2] = eucNotImplemented;
			} else {
				uint8_t responseSize = 0;
				response[2] = iFunction(&payload[0], payloadSize, &response[4], &responseSize);
				response[3] = responseSize;
				*length = responseSize + 4;
			}
		}
	}
}

void UARTBeginTransaction() {
	state = usProtoVer;
}

bool UARTProcessNextByte(unsigned char b, uint8_t *respBuffer, uint8_t *respSize) {
	switch (state) {
		case usEnd: {
			break;
		}
		case usError: {
			break;
		}
		case usProtoVer: {
			if (PROTO_VERSION == b) {
				state = usCommand;
			} else {
				state = usError;
				generateErrorResponse(eucBadVersion, respBuffer, respSize);
				return true;
			}
			break;
		}
		case usCommand: {
			command = b;
			state = usPDataLength;
			break;
		}
		case usPDataLength: {
			if (MAX_DATA_LENGTH <= b) {
				state = usError;
				generateErrorResponse(eucMemoryError, respBuffer, respSize);
				return true;
			} else if (0 == b) {
				// no payload
				state = usEnd;
				generateResponse(respBuffer, respSize);
				return true;
			}
			state = usPData;
			payloadSize = b;
			position = 0;
			break;
		}
		case usPData: {
			payload[position++] = b;
			if (payloadSize <= position) {
				state = usEnd;
				generateResponse(respBuffer, respSize);
				return true;
			}
			break;
		}
	}
	return false;
}
