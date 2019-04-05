#define F_CPU 1200000UL
#define HIGH 1
#define LOW  0

#include <avr/io.h>
#include <util/delay.h>

#define MOSI PB0
#define MISO PB1
#define SCK  PB2
#define SS   PB3

void enable_ss(void) {
  PORTB |= (1 << SS);
}

void disable_ss(void) {
  PORTB &= ~(1 << SS);
}

void write_mosi(char value) {
  if (value)
    PORTB |= (1 << MOSI);
  else
    PORTB &= ~(1 << MOSI);
}

int read_mosi() {
  if (PINB & (1 << MOSI))
    return 1;
  else
    return 0;
}

char send_byte(char message) {
  char response = 0;
  char counter = 0x80;

  enable_ss();

  for (counter = 0x80; counter; counter >>= 1) {
    write_mosi((message & counter) ? HIGH : LOW);

    _delay_ms(20);
    PORTB |= (1 << SCK);

    if (read_mosi() == HIGH) {
      response |= counter;
    }

    _delay_ms(20);
    PORTB &= ~(1 << SCK);
  }

  disable_ss();

  return response;
}

void setup(void) {
  DDRB |= (1 << SS) | (1 << SCK) | (1 << MOSI); // make SS, SCK, and MOSI output
  DDRB &= ~(1 << MISO); // make MISO input

  PORTB = 0x00; // set all pins to zero, clearing the SS and SCK, and disabling the MISO pullup resistor.
}

void loop(void) {
  char message = 0x00;
  volatile char response;

  while (1) {
    response = send_byte(message);
    (message == 0xff) ? message : message++;
  }
}
