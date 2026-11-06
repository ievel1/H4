#include "../../Include/Timer/Timer.h"

// Initialiseringsfunktion til Timer 1 med dynamisk millisekund-beregning
void Timer1_Init_ms(uint16_t timeout_ms) {
  // 1. Nulstil register for en sikkerheds skyld
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 2. Vælg CTC-mode (Clear Timer on Compare Match)
  // WGM13:0 = 0100 -> WGM12 sættes i TCCR1B
  TCCR1B |= (1 << WGM12);

  // 3. Vælg Prescaler = 256
  // CS12 = 1, CS11 = 0, CS10 = 0 (giver prescaler 256)
  TCCR1B |= (1 << CS12);

  // 4. Beregn OCR1A-værdien ud fra ønskede millisekunder
  // Formel: Ticks = (F_CPU / Prescaler / 1000) * timeout_ms
  // For 16 MHz og prescaler 256 er frekvensen ind i timeren: 16.000.000 / 256
  // = 62.500 Hz (62,5 ticks pr. ms)
  uint32_t ticks = ((F_CPU / 256UL) * timeout_ms) / 1000UL;

  if (ticks > 0) {
    ticks--; // Da sammenligningen starter fra 0
  }

  // Sørg for at værdien ikke overstiger 16-bit max (65535)
  if (ticks > 65535) {
    ticks = 65535;
  }

  OCR1A = (uint16_t)ticks;

  // 5. Aktivér Compare Match A Interrupt for Timer 1
  TIMSK1 |= (1 << OCIE1A);
}

// ISR (Interrupt Service Routine) for Timer 1 Compare Match A
ISR(TIMER1_COMPA_vect) {
  // Indsæt den kode, der skal udføres ved hvert timeout.
  // F.eks. toggling af en LED eller flag-sætning til hovedprogrammet.
  PORTB ^= (1 << GreenLedBit);
}