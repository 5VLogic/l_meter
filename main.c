#define F_CPU 16000000
#define ARRAY_LEN 6
#define STR_LEN 16
#define CONST 1010647491 // Approximate result of 4 pi^2 16MHz^2 C / 1'000
#define DELTA_MAX 20

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"

uint8_t i, flag, power, divisor;

uint16_t val[ARRAY_LEN];
uint16_t dmin, dmax;

uint32_t davg, davgsquared, L, Ldecimal, denominator;

char string[STR_LEN];


/* Interrupt service routine called by TC1 for every input capture signal.
 * Resets counter on first time being called.
 * Fills 'val' array with every subsequent call with absoulute time.
 * Updates flag every call to know how many times it has been called.
 */
ISR(TIMER1_CAPT_vect)
{
	if(flag == 0)
		TCNT1 = 0;

	else if(flag <= ARRAY_LEN)
		val[flag - 1] = ICR1;

	flag++;
}



void main()
{
	// Pins A0 - A5 outputs
	DDRC = 0b00111111;
	Lcd4_Init();
	Lcd4_Clear();
	Lcd4_Set_Cursor(0, 5);
	Lcd4_Write_String("L Meter");

	// Pin D1 as output (transistor base)
	DDRD |= (1 << PD1);

	// Enable global interrupt
	SREG |= (1 << 7);

	// Setup of AC
	ACSR &= ~(1 << ACD);// Enable AC
	ACSR &= ~(1 << ACBG);// Disable bandgap reference
	ADCSRB |= (1 << ACME);// Enable Mux
	ADCSRA &= ~(1 << ADEN);// Disable ADC
	ADMUX |= 0b00000111;// Select ADC7 pin as inverting input of AC


	// Setup of time TC1
	ACSR |= (1 << ACIC);// Select AC as TC1 input capture trigger
	TCCR1B |= 0b00000001;// Use prescaler = 1
	PRR &= ~(1 << PRTIM1);// Disable Power reduction to TC1
	TCCR1B |= (1 << ICNC1);// Enable noise cancelling (optional)
	TIMSK1 |= (1 << ICIE1);// Input capture interrupt enable for TC1


	while(1){

		_delay_ms(500);

		flag = 0;

		// Set Pin D1 to HIGH
		PORTD |= (1 << PD1);

		// Delay for approx 250 ns
		asm("nop");
		asm("nop");
		asm("nop");


		TCNT1 = 0;
		// Pin D1 to LOW
		PORTD &= ~(1 << PD1);

		_delay_ms(100);

		// 'Divisor' is the number of valid periods measured
		divisor = ARRAY_LEN - 1;

		dmax = 0;
		dmin = 0xffff;

		// loop through all values (periods)
		for(i = ARRAY_LEN - 1; i > 0; i--)
		{
			// if a value is null, kepp track with "divisor" and skip loop
			if(val[i] == 0){
				divisor = i - 1;
				continue;
			}
			// Find maximum period
			if(val[i] - val[i - 1] > dmax)
				dmax = val[i] - val[i - 1];

			// Find minumum period
			if(val[i] - val[i -	1] < dmin)
				dmin = val[i] - val[i - 1];
		}

		// Calculate average period of oscillation
		davg = (uint32_t)(val[divisor] - val[0]) / ((uint32_t)divisor);
		davgsquared = davg * davg;
		denominator = CONST;

		// Find power of denominator such that L is n*100 e-m for decimal formatting
		L = davgsquared / denominator;
		for(power = 0; L < 100; power++)
		{
			denominator /= 10;
			L = davgsquared / denominator;
		}
		// Because of rounding error, L can go from 99 -> 1000, must catch!
		if(L >= 1000){
			L /=10;
			power--;
		}

		sprintf(string, "%d", (uint16_t)L);

		// Insert '.' at correct position inside the string representing the decimal
		if(power % 3)
		{
			for(i = STR_LEN - 1; i >= 4 - (power % 3); i--)
				string[i] = string[i - 1];

			string[i] = '.';
		}

		Lcd4_Clear();
		Lcd4_Set_Cursor(0, 4);

		// Check validity of the measurements
		if(dmin <= davg &&\
			davg <= dmax &&\
			dmin > 0 &&\
			((uint16_t)davg) / (dmax - dmin) >= DELTA_MAX)
		{
			Lcd4_Write_String(string);

			if(power >= 9)
				Lcd4_Write_String("nH");
			else if(power >= 6)
				Lcd4_Write_String("uH");
			else if(power >= 3)
				Lcd4_Write_String("mH");
		}

		else if(dmin == 0xffff)
			Lcd4_Write_String("Open");

		else
			Lcd4_Write_String("Error");

		for(i = 0; i < ARRAY_LEN; i++)
			val[i] = 0;

	}
}


