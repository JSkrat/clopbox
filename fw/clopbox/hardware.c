/*
 * hardware.c
 *
 * Created: 22.05.2021 17:59:12
 *  Author: Mintytail
 * we're taking PORTC as output of all 8 outlets
 * is not designed for more than 8 outputs!
 *
 * pwm period is 1kHz with 64 levels
 * 
 */ 

#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "data.h"

static uint8_t phase = 0;

void hardware_init() {
	// disable external clock to enable PORTC7
	ASSR &= ~(1 << EXCLK) & ~(1 << AS2);
	// setup all outputs
	DDRC = 0xFF;
	// setup a timer
	// mode CTC, clear after TCNT2 == OCR2A
	TCCR2A = (1 << WGM21) | (0 << WGM20);
	// prescaling 1, frequency 62500, 256 timer, period 976.5625Hz
	TCCR2B = (0 << CS22) | (0 << CS21) | (1 << CS20) | (0 << WGM22);
	// make it 1kHz exactly to count milliseconds as well
	OCR2A = 250-1;
	// enable overflow interrupt
	TIMSK2 = (1 << OCIE2A);
}


ISR(TIMER2_COMPA_vect) {
	phase += 1;
	if (64 <= phase) {
		phase = 0;
		// decrement once per period, so it is a milliseconds
		// TODO it appears it is once per 64ms, so it is in 64ms chunks
		/*for (int i = 0; i < output_num; i++) {
			if (outputs_timers[i]) {
				outputs_timers[i] -= 1;
			}
		}*/
	}
	uint8_t output = 0;
	bool i_output;
	for (int i = 0; i < output_num; i++) {
		output <<= 1;
		/*if (outputs_timers[i]) {
			i_output = outputs_timers_powers[i] <= phase;
		} else {*/
			i_output = outputs[i] <= phase;
		//}
		if (i_output) output |= 1;
	}
	PORTC = output;
}
