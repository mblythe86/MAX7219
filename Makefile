GCCFLAGS=-g -Os -Wall -Wextra -Werror -mmcu=atmega168 -ffunction-sections -fdata-sections -std=gnu99
LINKFLAGS=-Wl,-u,vfprintf -lprintf_flt -Wl,-u,vfscanf -lscanf_flt -lm -Wl,--gc-sections
AVRDUDEFLAGS=-c avr109 -p m168 -b 115200 -P /dev/ttyUSB0
LINKOBJECTS=../libnerdkits/delay.o ../libnerdkits/lcd.o ../libnerdkits/uart.o

PROJECT=LED_display

all:	${PROJECT}-upload

libraries:
	make -C ../libnerdkits

.SECONDARY:

%.o: %.c libraries
	avr-gcc ${GCCFLAGS} ${LINKFLAGS} -o $@ $< ${LINKOBJECTS}

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@
	
%.ass: %.o
	avr-objdump -S -d $< > $@
	
%-upload: %.hex
	#avrdude ${AVRDUDEFLAGS} -U flash:w:LED_display.hex:a
	avrdude ${AVRDUDEFLAGS} -e
	sleep 0.1
	avrdude ${AVRDUDEFLAGS} -D -U flash:w:$<:a

clean:
	rm -f ${PROJECT}.{hex,ass,o}
