#define F_CPU 16000000
#define ARRAY_LEN 6
#define FREQ (F_CPU/1000000)
#define STR_LEN 16
#define CONST 1010647491
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
	DDRC = 0b00111111;
	Lcd4_Init();
	Lcd4_Clear();
	Lcd4_Set_Cursor(0, 5);
	Lcd4_Write_String("L Meter");

	DDRD |= (1 << PD1);

	SREG |= (1 << 7);

	// Setup of AC
	ACSR &= ~(1 << ACD);
	ACSR &= ~(1 << ACBG);
	ADCSRB |= (1 << ACME);
	ADCSRA &= ~(1 << ADEN);
	ADMUX |= 0b00000111;

	// Setup of time TC1
	ACSR |= (1 << ACIC);
	TCCR1B |= 0b00000001;
	PRR &= ~(1 << PRTIM1);
	TCCR1B |= (1 << ICNC1);
	TIMSK1 |= (1 << ICIE1);

//	Lcd4_Set_Cursor(1, 5);
//	Lcd4_Write_String("420 uH");

	while(1){

		_delay_ms(500);
//		continue;

		flag = 0;

		PORTD |= (1 << PD1);

		// Delay for approx 250 ns
		asm("nop");
		asm("nop");
		asm("nop");


		TCNT1 = 0;
		PORTD &= ~(1 << PD1);

		_delay_ms(100);

		divisor = ARRAY_LEN - 1;

		dmax = 0;
		dmin = 0xffff;

		for(i = ARRAY_LEN - 1; i > 0; i--)
		{
			if(val[i] == 0){
				divisor = i - 1;
				continue;
			}
			if(val[i] - val[i - 1] > dmax)
				dmax = val[i] - val[i - 1];

			if(val[i] - val[i -	1] < dmin)
				dmin = val[i] - val[i - 1];
		}

		davg = (uint32_t)(val[divisor] - val[0]) / ((uint32_t)divisor);
		davgsquared = davg * davg;
		denominator = CONST;

		L = davgsquared / denominator;
		for(power = 0; L < 100; power++)
		{
			denominator /= 10;
			L = davgsquared / denominator;
		}
		if(L >= 1000){
			L /=10;
			power--;
		}

		sprintf(string, "%d", (uint16_t)L);

		if(power % 3)
		{
			for(i = STR_LEN - 1; i >= 4 - (power % 3); i--)
				string[i] = string[i - 1];

			string[i] = '.';
		}

		Lcd4_Clear();
		Lcd4_Set_Cursor(0, 4);

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
				Lcd4_Write_String("nH");
		}

		else if(dmin == 0xffff)
			Lcd4_Write_String("Open");

		else
			Lcd4_Write_String("Error");

		for(i = 0; i < ARRAY_LEN; i++)
			val[i] = 0;

	}
}


