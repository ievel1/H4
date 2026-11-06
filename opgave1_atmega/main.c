/**
 * @file main.c
 * @author ltpe
 * @date 2026-11-05
 * @brief Main function
 */
#include "Include/Timer/Timer.h"
#include "Include/UART/uart.h"

int main(void) {
  DDRB |= (1 << GreenLedBit); // Sæt PB1 som output for den grønne LED

  // Eksempel 1: Sæt timeout til 500 ms
  Timer1_Init_ms(500);

  // Hvis du vil skifte til 250ms eller 1000ms, kalder du blot funktionen igen:
  // Timer1_Init_ms(250);
  // Timer1_Init_ms(1000);

  RS232Init();
  Enable_UART_Receive_Interrupt();


  // Aktivér globale interrupts
  sei();

  while (1) {
    // Hovedloop kan køre her uafhængigt
  }
}
