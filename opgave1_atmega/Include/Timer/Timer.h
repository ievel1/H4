#define F_CPU 16000000UL  // Antager 16 MHz krystal (ret hvis din kører med f.eks. 8 MHz)
#define GreenLedBit PORTB1  // Antag at den grønne LED er tilsluttet pin PB5
#include <avr/io.h>
#include <avr/interrupt.h>


extern void Timer1_Init_ms(uint16_t timeout_ms);