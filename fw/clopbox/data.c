/*
 * data.c
 *
 * Created: 22.05.2021 17:35:26
 *  Author: Mintytail
 */ 

#include <string.h> // for the memset
#include "data.h"

uint8_t outputs[output_num];
uint16_t outputs_timers[output_num];
uint8_t outputs_timers_powers[output_num];

void data_init() {
	memset(&outputs, 0, sizeof(outputs));
	memset(&outputs_timers, 0, sizeof(outputs_timers));
	memset(&outputs_timers_powers, 0, sizeof(outputs_timers_powers));
}
