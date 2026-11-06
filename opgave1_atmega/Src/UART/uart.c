/*
 * Uart.c
 *
 * Created: 15-01-2018 09:44:48
 *  Author: ltpe
 */

#include "../../Include/UART/uart.h"
#include "../../Include/ProjectDefines.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
// #include "../../Include/Main.h"

static FILE uart_output = FDEV_SETUP_STREAM(uart_putch, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getch, _FDEV_SETUP_READ);

void RS232Init(void) {
  // Set baud rate
  UBRR0L = (unsigned char)(MyUBRR & 0xff);
  UBRR0H = (unsigned char)(MyUBRR >> 8);
#ifdef UART_NORMAL_MODE
  UCSR0A = 0x00;
#else
  UCSR0A = 1 << U2X; // Set *2 mode
#endif
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // 0x18 , enable receive and transmit
  UCSR0C = (1 << UCSZ11) | (1 << UCSZ10); // 8 data bit 1 stop bit
  stdout = &uart_output;
  stdin = &uart_input;
}

void Enable_UART_Receive_Interrupt() {
  UCSR0B |=
      (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
}

void Disable_UART_Receive_Interupt() {
  UCSR0B &= ~(
      1 << RXCIE0); // Disable the USART Recieve Complete interrupt (USART_RXC)
}
static Function_Pointer_With_One_CharPointer_Parameter Callback_Function_Pointer = NULL;
 
void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter 
                                      Function_To_Callback)
{
	Callback_Function_Pointer = Function_To_Callback;
}

int uart_getch(FILE *stream) {
  while (!(UCSR0A & (1 << RXC0)))
    ; // do nothing but wait
  return (UDR0);
}

int uart_putch(char ch, FILE *stream) {
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = ch;
  return 0;
}

ISR(USART0_RX_vect) {
  char ReceivedByte;
  ReceivedByte = UDR0;

  if (ReceivedByte >= 'A' && ReceivedByte <= 'Z') {
    ReceivedByte = ReceivedByte + ('a' - 'A');
  } else if (ReceivedByte >= 'a' && ReceivedByte <= 'z') {
    ReceivedByte = ReceivedByte - ('a' - 'A');
  }

  while (!(UCSR0A & (1 << UDRE0)))
    ; // Wait until the UART is ready to send
  UDR0 = ReceivedByte; // Send the converted character back to the computer
}