
exe.ihex: exe.elf
	avr-objcopy -O ihex -R .eeprom exe.elf exe.ihex

exe.elf: main.c lcd.h
	avr-gcc -mmcu=atmega328p -O3 main.c -o exe.elf


u: exe.ihex
	sudo avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:exe.ihex:i

asm: main.c lcd.h
	avr-gcc -mmcu=atmega328p -S -O3 main.c -o main.asm




test.ihex: test.elf
	avr-objcopy -O ihex -R .eeprom test.elf test.ihex

test.elf: testmain.c lcd.h
	avr-gcc -mmcu=atmega328p -O3 testmain.c -o test.elf

utest: test.ihex
	sudo avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:test.ihex:i

