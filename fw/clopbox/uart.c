/*
 * uart.c
 *
 * Created: 22.05.2021 19:35:27
 *  Author: Mintytail
 * usb connected to UART0 at PD0, PD1
 */ 
#include <avr/interrupt.h>
#include "uart.h"
#include "data.h"
#include "uart protocol.h"

#define U_TRANSMIT_START UCSR0B |= (1 << UDRIE0)
#define SENDBUFFER_SIZE 64
static uint8_t sendBuffer[SENDBUFFER_SIZE];
static uint8_t sendBufferBegin = 0, sendBufferEnd = 0;
static bool escActive;
static uint8_t respBuffer[SENDBUFFER_SIZE], respSize = 0;

void uart_init() {
	DDRD |= (1 << PORTD1);
	// double the speed
	UCSR0A = (1 << U2X0);
	// enable uart and interrupts
	UCSR0B = (1 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02);
	// asynchronous uart, no parity, 1 stop bit, 8 bit character
	UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
	// baud 115000, UBRR = 16 for 16MHz (+2.1% error)
	UBRR0H = 0;
	UBRR0L = 16;
}

void queueChar(const uint8_t c) {
	sendBuffer[sendBufferEnd] = c;
	// separately, so no garbage is sent
	sendBufferEnd++;
	if (SENDBUFFER_SIZE <= sendBufferEnd) sendBufferEnd = 0;
}

void queueBytes(const uint8_t *bytes, const uint8_t number) {
	for (uint8_t p = 0; p < number; p++) {
		queueChar(bytes[p]);
	}
}

void uSendFrame(const uint8_t *bytes, const uint8_t number) {
	// frame start
	queueChar(0xC0);
	for (int8_t i = 0; i < number; i++) {
		uint8_t c = bytes[i];
		if (0xC0 == c) {
			queueChar(0xDB);
			c = 0xDC;
		} else if (0xDB == c) {
			queueChar(0xDB);
			c = 0xDD;
		}
		queueChar(c);
	}
	U_TRANSMIT_START;
}

void parseFrame(unsigned char b) {
	switch (b) {
		case 0xC0: {
			UARTBeginTransaction();
			escActive = false;
			return;
		}
		case 0xDB: {
			escActive = true;
			return;
		}
		case 0xDC: {
			if (escActive) {
				b = 0xC0;
				escActive = false;
			}
			break;
		}
		case 0xDD: {
			if (escActive) {
				b = 0xDB;
				escActive = false;
			}
			break;
		}
	}
	if (UARTProcessNextByte(b, &respBuffer[0], &respSize)) {
		uSendFrame(respBuffer, respSize);
	}
}

ISR(USART0_RX_vect) {
	parseFrame(UDR0);
}

ISR(USART0_UDRE_vect) {
	if (sendBufferBegin == sendBufferEnd) {
		UCSR0B &= ~(1 << UDRIE0);
		return;
	}
	UDR0 = sendBuffer[sendBufferBegin++];
	if (SENDBUFFER_SIZE <= sendBufferBegin) sendBufferBegin = 0;
}
