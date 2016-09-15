#include <SPI.h>

// max7219 registers
#define MAX7219_REG_NOOP         0x0
#define MAX7219_REG_DIGIT0       0x1
#define MAX7219_REG_DIGIT1       0x2
#define MAX7219_REG_DIGIT2       0x3
#define MAX7219_REG_DIGIT3       0x4
#define MAX7219_REG_DIGIT4       0x5
#define MAX7219_REG_DIGIT5       0x6
#define MAX7219_REG_DIGIT6       0x7
#define MAX7219_REG_DIGIT7       0x8
#define MAX7219_REG_DECODEMODE   0x9
#define MAX7219_REG_INTENSITY    0xA
#define MAX7219_REG_SCANLIMIT    0xB
#define MAX7219_REG_SHUTDOWN     0xC
#define MAX7219_REG_DISPLAYTEST  0xF

//pin 16 can't interrupt, so use it as SPI chip select
#define CS_PIN D0

void spiSend (const byte reg, const byte data) {
  // enable the line
  digitalWrite(CS_PIN, 0);
  // now shift out the data
  SPI.transfer (reg);
  SPI.transfer (data);
  digitalWrite (CS_PIN, 1);

}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(100);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite (CS_PIN, 1);

  SPI.begin ();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  spiSend(MAX7219_REG_SCANLIMIT, 3);   // show only 4 digits
  spiSend(MAX7219_REG_DECODEMODE, 1);  // using digits
  spiSend(MAX7219_REG_DISPLAYTEST, 0); // no display test
  spiSend(MAX7219_REG_INTENSITY, 15);   // character intensity: range: 0 to 15
  spiSend(MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (ie. start it up)

  pinMode(CS_PIN, OUTPUT);
  digitalWrite (CS_PIN, 1);
}

const byte patterns[] = { 0b00111110,
                          0b01011110,
                          0b01101110,
                          0b01110110,
                          0b01111010,
                          0b01111100
                        };

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Seg test");
  spiSend(MAX7219_REG_DISPLAYTEST, 1); // display test
  delay(500);

  Serial.println("Shutdown");
  spiSend(MAX7219_REG_SHUTDOWN, 0);    // in shutdown mode
  spiSend(MAX7219_REG_DISPLAYTEST, 0); // no display test
  delay(500);

  Serial.println("Count");
  spiSend(MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (ie. start it up)
  spiSend(MAX7219_REG_DECODEMODE, 0xf);  // using digits
  for (byte count = 0; count < 16; count++) {
    for (byte digit = 1; digit <= 4; digit++) {
      spiSend(digit, digit + count-1);
    }
    delay(500);
  }

  Serial.println("Spin");
  spiSend(MAX7219_REG_DECODEMODE, 0);  // no digit decode
  for (byte spin = 0; spin < 4; spin++) {
    for (byte i = 0; i < 6; i++) {
      for (byte digit = 1; digit < 5; digit++) {
        spiSend(digit, patterns[i]);
      }
      delay(50);
    }
  }
  
  Serial.println("Spin2");
  spiSend(MAX7219_REG_DECODEMODE, 0);  // no digit decode
  for (byte spin = 0; spin < 4; spin++) {
    for (byte i = 0; i < 6; i++) {
      for (byte digit = 1; digit < 5; digit++) {
        spiSend(digit, patterns[i]^0b01111110);
      }
      delay(50);
    }
  }

}
