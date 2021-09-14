/*
 * uart_functions.c
 *
 * Created: 23.05.2021 00:42:54
 *  Author: Mintytail
 */ 

#include <avr/pgmspace.h>
#include <stddef.h> // for NULL
#include "uart functions.h"
#include "uart protocol.h"
#include "data.h"


uint8_t version(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize) {
	*responseSize = 4;
	return eucOk;
}

uint8_t resetOutputs(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize) {
	for (int i = 0; i < output_num; i++) {
		outputs[i] = 0;
	}
	return eucOk;
}

uint8_t setOutput(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize) {
	// 0th is output number
	// 1st is power
	// validation
	if (2 != payloadSize) return eucArgumentValidationError;
	if (output_num <= payload[0]) return eucArgumentValidationError;
	if (MAX_POWER <= payload[1]) return eucArgumentValidationError;
	outputs[payload[0]] = payload[1];
	return eucOk;
}

uint8_t getOutputs(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize) {
	for (int i = 0; i < output_num; i++) {
		response[i] = outputs[i];
	}
	*responseSize = output_num;
	return eucOk;
}

uint8_t setOutputWithTimeout(const uint8_t *payload, const uint8_t payloadSize, uint8_t *response, uint8_t *responseSize) {
	// 0th is output number
	// 1st is power
	// 2nd and 3rd is time in milliseconds
	// after timeout it will return to its original value, set by setOutput
	// validation
	if (4 != payloadSize) return eucArgumentValidationError;
	if (output_num <= payload[0]) return eucArgumentValidationError;
	if (MAX_POWER <= payload[1]) return eucArgumentValidationError;
	outputs_timers_powers[payload[0]] = payload[1];
	outputs_timers[payload[0]] = (payload[3] << 8) + payload[2];
	return eucOk;
}

const PROGMEM tUARTCommandItem UARTFunctions[UART_FUNCTIONS_NUMBER] = {
	{ ufEcho, NULL },
	{ ufVersion, NULL },
	{ ufResetOutputs, &resetOutputs },
	{ ufSetOutput, &setOutput },
	{ ufGetOutputs, &getOutputs },
	{ ufSetOutputWithTimeout, &setOutputWithTimeout },
};

