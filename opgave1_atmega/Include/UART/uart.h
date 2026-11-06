/*
 * Uart.h
 *
 * Created: 15-01-2018 09:45:07
 *  Author: ltpe
 */ 

#include "../ProjectDefines.h"

#ifndef UART_H_
#define UART_H_

//#include "Arduino.h"
#include <stdio.h>
#define BAUD 9600UL

#define UART_NORMAL_MODE
#ifdef UART_NORMAL_MODE
#define MyUBRR (unsigned int)(F_CPU/(16*BAUD) -1)   // normal mode
#else
#define MyUBRR (unsigned int)(F_CPU/(8*BAUD) -1)   // *2 mode
#endif

void SetupUARTFunctionCallbackPointer(Function_Pointer_With_One_CharPointer_Parameter 
                                      Function_To_Callback);
extern void RS232Init(void);
extern int uart_getch(FILE* stream);
extern int uart_putch(char ch, FILE* stream);
extern void Enable_UART_Receive_Interrupt();
extern void Disable_UART_Receive_Interupt();

#endif
