/**
 * @file main.c
 * @brief Timer1 LED blink, UART0 receive interrupt and OLED text output.
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define DELAY_MS(ms) __builtin_avr_delay_cycles((F_CPU / 1000UL) * (ms))
#define UART_BUFFER_SIZE 64
#define OLED_TEXT_LENGTH 10
#define INPUT_BUFFER_SIZE 128

static volatile uint8_t uart_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart_head;
static volatile uint8_t uart_tail;
static uint8_t oled_address;

static const uint8_t font[26][5] = {
    {0x7E, 0x09, 0x09, 0x09, 0x7E}, {0x7F, 0x49, 0x49, 0x49, 0x36},
    {0x3E, 0x41, 0x41, 0x41, 0x22}, {0x7F, 0x41, 0x41, 0x22, 0x1C},
    {0x7F, 0x49, 0x49, 0x49, 0x41}, {0x7F, 0x09, 0x09, 0x09, 0x01},
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, {0x7F, 0x08, 0x08, 0x08, 0x7F},
    {0x00, 0x41, 0x7F, 0x41, 0x00}, {0x20, 0x40, 0x41, 0x3F, 0x01},
    {0x7F, 0x08, 0x14, 0x22, 0x41}, {0x7F, 0x40, 0x40, 0x40, 0x40},
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, {0x7F, 0x04, 0x08, 0x10, 0x7F},
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, {0x7F, 0x09, 0x09, 0x09, 0x06},
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, {0x7F, 0x09, 0x19, 0x29, 0x46},
    {0x46, 0x49, 0x49, 0x49, 0x31}, {0x01, 0x01, 0x7F, 0x01, 0x01},
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, {0x1F, 0x20, 0x40, 0x20, 0x1F},
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, {0x63, 0x14, 0x08, 0x14, 0x63},
    {0x07, 0x08, 0x70, 0x08, 0x07}, {0x61, 0x51, 0x49, 0x45, 0x43}
};

static const uint8_t digits[10][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, {0x00, 0x42, 0x7F, 0x40, 0x00},
    {0x42, 0x61, 0x51, 0x49, 0x46}, {0x21, 0x41, 0x45, 0x4B, 0x31},
    {0x18, 0x14, 0x12, 0x7F, 0x10}, {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, {0x01, 0x71, 0x09, 0x05, 0x03},
    {0x36, 0x49, 0x49, 0x49, 0x36}, {0x06, 0x49, 0x49, 0x29, 0x1E}
};

static const uint8_t special[8][5] = {
    {0x38, 0x54, 0x54, 0x54, 0x58}, /* ae */
    {0x38, 0x44, 0x54, 0x4C, 0x38}, /* oe */
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, /* aa */
    {0x08, 0x3E, 0x49, 0x49, 0x08}, /* inverted exclamation */
    {0x3E, 0x21, 0x7F, 0x21, 0x12}, /* pound */
    {0x14, 0x55, 0x36, 0x14, 0x00}, /* half */
    {0x22, 0x14, 0x7F, 0x14, 0x08}, /* yen */
    {0x14, 0x7F, 0x14, 0x7F, 0x14}  /* plus-minus */
};

static void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 7812;  /* approximately 500 ms at 16 MHz / 1024 */
    TIMSK1 = (1 << OCIE1A);
}

static void uart0_init(void)
{
    UBRR0H = 0;
    UBRR0L = 103;  /* 9600 baud at 16 MHz */
    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart0_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0))) {
    }
    UDR0 = data;
}

static uint8_t uart0_read(void)
{
    uint8_t data;

    if (uart_head == uart_tail) {
        return 0;
    }

    data = uart_buffer[uart_tail];
    uart_tail = (uart_tail + 1) % UART_BUFFER_SIZE;
    return data;
}

ISR(TIMER1_COMPA_vect)
{
    PORTB ^= (1 << PB2) | (1 << PB3);
}

ISR(USART_RX_vect)
{
    uint8_t next_head = (uart_head + 1) % UART_BUFFER_SIZE;

    if (next_head != uart_tail) {
        uart_buffer[uart_head] = UDR0;
        uart_head = next_head;
    } else {
        (void)UDR0;
    }
}

static void twi_init(void)
{
    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC |= (1 << PC4) | (1 << PC5);
    TWSR = 0;
    TWBR = 72;
    TWCR = (1 << TWEN);
}

static uint8_t twi_start(uint8_t address)
{
    uint16_t timeout = 0;

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)) && timeout++ < 60000) {
    }
    if (!(TWCR & (1 << TWINT))) {
        return 0;
    }

    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    timeout = 0;
    while (!(TWCR & (1 << TWINT)) && timeout++ < 60000) {
    }
    return (TWCR & (1 << TWINT)) && (TWSR & 0xF8) == 0x18;
}

static void twi_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static uint8_t twi_write(uint8_t data)
{
    uint16_t timeout = 0;

    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)) && timeout++ < 60000) {
    }
    return (TWCR & (1 << TWINT)) && (TWSR & 0xF8) == 0x28;
}

static uint8_t twi_probe(uint8_t address)
{
    uint8_t found = twi_start(address << 1);
    twi_stop();
    return found;
}

static uint8_t oled_command(uint8_t command)
{
    if (!twi_start(oled_address << 1) ||
        !twi_write(0x00) ||
        !twi_write(command)) {
        twi_stop();
        return 0;
    }
    twi_stop();
    return 1;
}

static uint8_t oled_init(void)
{
    static const uint8_t commands[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6,
        0xAF
    };

    for (uint8_t i = 0; i < sizeof(commands); i++) {
        if (!oled_command(commands[i])) {
            return 0;
        }
    }
    return 1;
}

static uint8_t oled_clear(void)
{
    if (!oled_command(0x21) || !oled_command(0) || !oled_command(127) ||
        !oled_command(0x22) || !oled_command(0) || !oled_command(7) ||
        !twi_start(oled_address << 1) || !twi_write(0x40)) {
        twi_stop();
        return 0;
    }

    for (uint16_t i = 0; i < 1024; i++) {
        if (!twi_write(0)) {
            twi_stop();
            return 0;
        }
    }
    twi_stop();
    return 1;
}

static uint8_t font_column(uint8_t character, uint8_t column)
{
    if (character >= 'a' && character <= 'z') {
        character -= 'a' - 'A';
    }
    if (character >= 'A' && character <= 'Z') {
        return font[character - 'A'][column];
    }
    if (character >= '0' && character <= '9') {
        return digits[character - '0'][column];
    }
    if (character >= 0x80 && character <= 0x82) {
        return special[character - 0x80][column];
    }
    if (character >= 0x83 && character <= 0x87) {
        return special[character - 0x80][column];
    }
    if (character == ' ') {
        return 0;
    }
    if (character == '.') {
        return column == 2 ? 0x40 : 0;
    }
    if (character == ',') {
        return column == 2 ? 0xC0 : 0;
    }
    if (character == '!') {
        return column == 2 ? 0x7D : 0;
    }
    if (character == '-') {
        return (column == 1 || column == 2 || column == 3) ? 0x08 : 0;
    }
    if (character == '+') {
        return (column == 2) ? 0x1C : ((column == 1 || column == 3) ? 0x08 : 0);
    }
    if (character == ':') {
        return (column == 2) ? 0x24 : 0;
    }
    if (character == '/') {
        return (column == 0) ? 0x60 :
               (column == 1) ? 0x18 :
               (column == 3) ? 0x06 :
               (column == 4) ? 0x03 : 0;
    }
    if (character == '@') {
        static const uint8_t glyph[] = {0x3E, 0x41, 0x5D, 0x55, 0x1E};
        return glyph[column];
    }
    if (character == '$') {
        static const uint8_t glyph[] = {0x24, 0x2A, 0x7F, 0x2A, 0x12};
        return glyph[column];
    }
    if (character == '"') {
        static const uint8_t glyph[] = {0x00, 0x03, 0x00, 0x03, 0x00};
        return glyph[column];
    }
    if (character == '#') {
        static const uint8_t glyph[] = {0x14, 0x7F, 0x14, 0x7F, 0x14};
        return glyph[column];
    }
    if (character == '%') {
        static const uint8_t glyph[] = {0x63, 0x13, 0x08, 0x64, 0x63};
        return glyph[column];
    }
    if (character == '&') {
        static const uint8_t glyph[] = {0x36, 0x49, 0x55, 0x22, 0x50};
        return glyph[column];
    }
    if (character == '\'') {
        return column == 2 ? 0x03 : 0;
    }
    if (character == '(') {
        static const uint8_t glyph[] = {0x00, 0x1C, 0x22, 0x41, 0x00};
        return glyph[column];
    }
    if (character == ')') {
        static const uint8_t glyph[] = {0x00, 0x41, 0x22, 0x1C, 0x00};
        return glyph[column];
    }
    if (character == '*') {
        static const uint8_t glyph[] = {0x14, 0x08, 0x3E, 0x08, 0x14};
        return glyph[column];
    }
    if (character == ';') {
        return column == 2 ? 0x64 : 0;
    }
    if (character == '=') {
        return (column == 1 || column == 2 || column == 3) ? 0x14 : 0;
    }
    if (character == '<') {
        static const uint8_t glyph[] = {0x08, 0x14, 0x22, 0x41, 0x00};
        return glyph[column];
    }
    if (character == '>') {
        static const uint8_t glyph[] = {0x00, 0x41, 0x22, 0x14, 0x08};
        return glyph[column];
    }
    if (character == '[' || character == ']') {
        static const uint8_t left[] = {0x00, 0x7F, 0x41, 0x41, 0x00};
        static const uint8_t right[] = {0x00, 0x41, 0x41, 0x7F, 0x00};
        return (character == '[' ? left : right)[column];
    }
    if (character == '\\') {
        static const uint8_t glyph[] = {0x03, 0x06, 0x0C, 0x18, 0x60};
        return glyph[column];
    }
    if (character == '^') {
        static const uint8_t glyph[] = {0x04, 0x02, 0x01, 0x02, 0x04};
        return glyph[column];
    }
    if (character == '_') {
        return column == 2 ? 0x40 : 0;
    }
    if (character == '`') {
        return column == 1 ? 0x03 : 0;
    }
    if (character == '{' || character == '}') {
        static const uint8_t left[] = {0x08, 0x36, 0x41, 0x00, 0x00};
        static const uint8_t right[] = {0x00, 0x00, 0x41, 0x36, 0x08};
        return (character == '{' ? left : right)[column];
    }
    if (character == '|') {
        return column == 2 ? 0x7F : 0;
    }
    if (character == '~') {
        return (column == 1 || column == 3) ? 0x08 : 0x04;
    }
    if (character == '?') {
        return (column == 0 || column == 4) ? 0x01 :
               (column == 1 || column == 3) ? 0x48 :
               (column == 2) ? 0x49 : 0;
    }
    return column == 2 ? 0x5D : 0x41;
}

static uint8_t oled_write_text(const uint8_t *text)
{
    for (uint8_t page = 2; page <= 3; page++) {
        if (!oled_command(0x21) || !oled_command(2) || !oled_command(121) ||
            !oled_command(0x22) || !oled_command(page) ||
            !oled_command(page) || !twi_start(oled_address << 1) ||
            !twi_write(0x40)) {
            twi_stop();
            return 0;
        }

        for (uint8_t character = 0; character < OLED_TEXT_LENGTH; character++) {
            for (uint8_t column = 0; column < 5; column++) {
                uint16_t scaled = 0;

                for (uint8_t row = 0; row < 7; row++) {
                    if (font_column(text[character], column) & (1 << row)) {
                        scaled |= (uint16_t)3 << (2 * row);
                    }
                }

                for (uint8_t repeat = 0; repeat < 2; repeat++) {
                    if (!twi_write(page == 2 ? (uint8_t)scaled :
                                             (uint8_t)(scaled >> 8))) {
                        twi_stop();
                        return 0;
                    }
                }
            }

            if (!twi_write(0) || !twi_write(0)) {
                twi_stop();
                return 0;
            }
        }

        twi_stop();
    }

    return 1;
}

static void append_text(uint8_t *text, uint8_t *length, uint8_t character)
{
    if (*length == INPUT_BUFFER_SIZE) {
        for (uint8_t i = 1; i < INPUT_BUFFER_SIZE; i++) {
            text[i - 1] = text[i];
        }
        *length = INPUT_BUFFER_SIZE - 1;
    }

    text[(*length)++] = character;
}

static void uart_send_swapped_case_line(const uint8_t *text, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++) {
        uint8_t character = text[i];

        if (character >= 'a' && character <= 'z') {
            character -= 'a' - 'A';  /* lowercase becomes uppercase */
        } else if (character >= 'A' && character <= 'Z') {
            character += 'a' - 'A';  /* uppercase becomes lowercase */
        } else if (character == 0x80) {
            uart0_send('A');
            uart0_send('E');
            continue;
        } else if (character == 0x81) {
            uart0_send('O');
            uart0_send('E');
            continue;
        } else if (character == 0x82) {
            uart0_send('A');
            uart0_send('A');
            continue;
        }

        uart0_send(character);
    }
}


int main(void)
{
    uint8_t text[INPUT_BUFFER_SIZE];
    uint8_t text_length = 0;
    uint8_t utf8_pending = 0;
    uint8_t suppress_lf = 0;

    DDRB |= (1 << PB2) | (1 << PB3);
    PORTB |= (1 << PB2) | (1 << PB3);
    for (uint8_t i = 0; i < INPUT_BUFFER_SIZE; i++) {
        text[i] = ' ';
    }

    twi_init();
    uart0_init();
    timer1_init();
    sei();

    if (twi_probe(0x3C)) {
        oled_address = 0x3C;
    } else if (twi_probe(0x3D)) {
        oled_address = 0x3D;
    }

    if (oled_address != 0 && oled_init()) {
        oled_clear();
        oled_write_text(text);
    }

    while (1) {
        uint8_t received = uart0_read();

        if (received != 0) {
            if (utf8_pending) {
                uint8_t prefix = utf8_pending;
                utf8_pending = 0;
                uart0_send(received);
                if (prefix == 0xC3 &&
                    (received == 0xA6 || received == 0x86)) {
                    received = 0x80;  /* ae / AE */
                } else if (prefix == 0xC3 &&
                           (received == 0xB8 || received == 0x98)) {
                    received = 0x81;  /* oe / OE */
                } else if (prefix == 0xC3 &&
                           (received == 0xA5 || received == 0x85)) {
                    received = 0x82;  /* aa / AA */
                } else if (prefix == 0xC2 && received == 0xA1) {
                    received = 0x83;  /* inverted exclamation */
                } else if (prefix == 0xC2 && received == 0xA3) {
                    received = 0x84;  /* pound */
                } else if (prefix == 0xC2 && received == 0xBD) {
                    received = 0x85;  /* half */
                } else if (prefix == 0xC2 && received == 0xA5) {
                    received = 0x86;  /* yen */
                } else if (prefix == 0xC2 && received == 0xB1) {
                    received = 0x87;  /* plus-minus */
                } else {
                    received = '?';
                }
                append_text(text, &text_length, received);
            } else if (received == 0xC2 || received == 0xC3) {
                utf8_pending = received;
                uart0_send(received);
            } else if (received == '\b' || received == 0x7F) {
                uart0_send(received);
                if (text_length > 0) {
                    text[--text_length] = ' ';
                }
            } else if (received == '\r' || received == '\n') {
                if (!(received == '\n' && suppress_lf)) {
                    uart0_send('\r');
                    uart0_send('\n');
                    uart_send_swapped_case_line(text, text_length);
                    uart0_send('\r');
                    uart0_send('\n');
                }
                suppress_lf = received == '\r';
                text_length = 0;
                for (uint8_t i = 0; i < INPUT_BUFFER_SIZE; i++) {
                    text[i] = ' ';
                }
            } else {
                uart0_send(received);
                suppress_lf = 0;
                append_text(text, &text_length, received);
            }

            if (oled_address != 0) {
                uint8_t start = text_length > OLED_TEXT_LENGTH
                                    ? text_length - OLED_TEXT_LENGTH : 0;
                oled_clear();
                oled_write_text(&text[start]);
            }
        }
    }
}
