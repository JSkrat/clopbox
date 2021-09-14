/*
 * clopbox.c
 *
 * Created: 22.05.2021 14:10:44
 * Author : Mintytail
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "data.h"
#include "hardware.h"
#include "uart.h"


int main(void)
{
	data_init();
	hardware_init();
	uart_init();
	for (int i = 0; i < output_num; i++) {
		outputs[i] = 0;
	}
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}

