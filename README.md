## Inductance meter for Arduino Nano (ATmega328p)
# Timer TC1
The Timer/Counter 1 is used to time the period of oscillation
of the LC circuit over several full periods.
The interrupt routine stores each absolute time value
in an array so the difference can be then computed.

# Analog Comparator AC
The analog comparator detects the zero crossing for each period.
It is connected to TC1 and triggers it's ISR.
Pin A7 is the inverting input and pin D6 the non-inverting one.

# LCD
The LED used is a common 16x2 display and is controlled by the
functions defined in lcd.h.
The LCD pins are A0-A5 (defined in lcd.h).
