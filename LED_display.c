// tempsensor.c
// for NerdKits with ATmega168
// mrobbins@mit.edu

#define F_CPU 14745600

#include <stdio.h>
#include <math.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "../libnerdkits/delay.h"
#include "../libnerdkits/lcd.h"
#include "../libnerdkits/uart.h"

#define Ob(x)  ((unsigned)Ob_(0 ## x ## uL))
#define Ob_(x) ((x & 1) | (x >> 2 & 2) | (x >> 4 & 4) | (x >> 6 & 8) |		\
	(x >> 8 & 16) | (x >> 10 & 32) | (x >> 12 & 64) | (x >> 14 & 128))

// PIN DEFINITIONS:
// (In order-ish on chip)
// 15 PB1 -- 
// 16 PB2 -- SS
// 17 PB3 -- MOSI
// 18 PB4 -- MISO
// 19 PB5 -- SCK
// 20 AVCC -- +5V
// 21 AREF -- +5V
// 22 GND
// 23 PC0 -- 
// 24 PC1 -- 
// 25 PC2 -- 
// 26 PC3 -- 
// 27 PC4 -- 
// 28 PC5 -- 
// 
// 14 PB0 -- Bootloading pin (pull to gnd for programming)
// 13 PD7 -- LCD Register Select (!cmd/data)
// 12 PD6 -- LCD Clock
// 11 PD5 -- LCD Data bit 7
// 10 PB7 -- XTAL2
//  9 PB6 -- XTAL1
//  8 GND
//  7 VCC -- +5V
//  6 PD4 -- LCD Data bit 6
//  5 PD3 -- LCD Data bit 5
//  4 PD2 -- LCD Data bit 4
//  3 PD1 -- UART TX (green)
//  2 PD0 -- UART RX (yellow)
//  1 PC6 -- !reset (+5V)

FILE lcd_stream;
FILE uart_stream;

// Initialize SPI Master Device (without interrupt)
void spi_init_master (void)
{
  // Set MOSI, SCK, SS as Output
  DDRB = (1<<5)|(1<<3)|(1<<2);

  // Enable SPI, Set as Master
  //Prescaler: Fosc/16, Enable Interrupts
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //|(1<<SPIE);
  //sei();

  // set SS high
  PORTB |= (1<<2);
  delay_ms(500);
}

//Function to send and receive data for both master and slave
void spi_send (unsigned char data0, unsigned char data1)
{
  // SS low
  PORTB &= ~(1<<2);
  //lcd_write_string(PSTR("."));

  // Load data into the buffer
  SPDR = data0;
  //lcd_write_string(PSTR("."));

  //Wait until transmission complete
  while(!(SPSR & (1<<SPIF) ));
  //lcd_write_string(PSTR("."));

  // Load data into the buffer
  SPDR = data1;
  //lcd_write_string(PSTR("."));

  //Wait until transmission complete
  while(!(SPSR & (1<<SPIF) ));
  //lcd_write_string(PSTR("."));

  // set SS high
  PORTB |= (1<<2);
  //lcd_write_string(PSTR("."));
}

uint8_t patterns[] = { 0b00111110,
    0b01011110,
    0b01101110,
    0b01110110,
    0b01111010,
    0b01111100 };

int main() {
  // start up the LCD
  lcd_init();
  FILE lcd_stream2 = FDEV_SETUP_STREAM(lcd_putchar, 0, _FDEV_SETUP_WRITE);
  lcd_stream = lcd_stream2;
  lcd_home(); 

  // start up the serial port
  uart_init();
  FILE uart_stream2 = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  uart_stream = uart_stream2;
  stdin = stdout = &uart_stream;

  spi_init_master(); 
  lcd_write_string(PSTR("SENDING"));

  //display test
  while(1){
    lcd_home(); 
    lcd_write_string(PSTR("SEG_TEST"));
    spi_send(0xf, 0x1);
    delay_ms(500);
    lcd_home(); 
    lcd_write_string(PSTR("SHUTDOWN"));
    spi_send(0xc, 0);   // enable shutdown mode
    spi_send(0xf, 0);
    delay_ms(500);
    lcd_home(); 
    lcd_write_string(PSTR("COUNT   "));
    spi_send(0xa, 0xf); //set intensity
    spi_send(0x9, 0xf); //set decode mode
    spi_send(0xb, 3);   //only scan digits 0-3
    spi_send(0xc, 1);   //disable shutdown mode
    for(uint8_t count=0; count<16; count++){
      for(uint8_t digit=1; digit<=4; digit++){
        spi_send(digit, digit+count);
      }
      delay_ms(500);
    }
    //spi_send(0x1, 0);
    //spi_send(0x2, 1);
    //spi_send(0x3, 2);
    //spi_send(0x4, 3);
    //delay_ms(500);
    lcd_home(); 
    lcd_write_string(PSTR("SPIN    "));
    spi_send(0x9, 0x0); //unset decode mode
    for(uint8_t spin=0; spin<4; spin++){
      for(uint8_t i=0; i<6; i++){
        //lcd_write_int16(patterns[i]);
        //lcd_write_data(' ');
	for(uint8_t digit=1; digit<5; digit++){
	  spi_send(digit, patterns[i]);
	}
	delay_ms(242);
      }
    }
    lcd_home(); 
    lcd_write_string(PSTR("SPIN2   "));
    spi_send(0x9, 0x0); //unset decode mode
    for(uint8_t spin=0; spin<4; spin++){
      for(uint8_t i=0; i<6; i++){
        //lcd_write_int16(patterns[i]);
        //lcd_write_data(' ');
	for(uint8_t digit=1; digit<5; digit++){
	  spi_send(digit, patterns[i] ^ 0b01111110);
	}
	delay_ms(250);
      }
    }
  }

  lcd_write_string(PSTR("SENT!"));

  return 0;
}
