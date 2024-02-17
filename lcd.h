//https://electrosome.com/interfacing-lcd-atmega32-microcontroller-atmel-studio/
//LCD Functions Developed by electroSome

// Pins must be defined in this file.
// If there are problems with writing strings,
//		increase delay in the Lcd4_Write_Char() to the default 1ms.



#define F_CPU 16000000

#include<util/delay.h>


void pinChange(uint8_t _PIN_, uint8_t _VAL_){
	((_VAL_ == 1) ? (PORTC |= (1 << _PIN_)) : (PORTC &= ~(1 << _PIN_)));
}



#define D4 2
#define D5 3
#define D6 4
#define D7 5
#define RS 0
#define EN 1

//LCD 4 Bit Interfacing Functions

void Lcd4_Port(char a)
{
	if(a & 1)
	pinChange(D4,1);
	else
	pinChange(D4,0);
	
	if(a & 2)
	pinChange(D5,1);
	else
	pinChange(D5,0);
	
	if(a & 4)
	pinChange(D6,1);
	else
	pinChange(D6,0);
	
	if(a & 8)
	pinChange(D7,1);
	else
	pinChange(D7,0);
}
void Lcd4_Cmd(char a)
{
	pinChange(RS,0);             // => RS = 0
	Lcd4_Port(a);
	pinChange(EN,1);            // => E = 1
	_delay_ms(1);
	pinChange(EN,0);             // => E = 0
	_delay_ms(1);
}

void Lcd4_Clear()
{
	Lcd4_Cmd(0);
	Lcd4_Cmd(1);
}

void Lcd4_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 0)
	{
		temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
	}
	else if(a == 1)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
	}
	Lcd4_Cmd(z);
	Lcd4_Cmd(y);
}

void Lcd4_Init()
{
	Lcd4_Port(0x00);
	_delay_ms(20);
	///////////// Reset process from datasheet /////////
	Lcd4_Cmd(0x03);
	_delay_ms(5);
	Lcd4_Cmd(0x03);
	_delay_ms(11);
	Lcd4_Cmd(0x03);
	/////////////////////////////////////////////////////
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x08);// 0x08
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x0c);// 0x0c
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x06);
}

void Lcd4_Write_Char(char a)
{
	char temp,y;
	temp = a&0x0F;
	y = a&0xF0;
	pinChange(RS,1);             // => RS = 1
	Lcd4_Port(y>>4);             //Data transfer
	pinChange(EN,1);
	//_delay_ms(1);
	_delay_us(1);
	pinChange(EN,0);
	//_delay_ms(1);
	_delay_us(1);
	Lcd4_Port(temp);
	pinChange(EN,1);
	//_delay_ms(1);
	_delay_us(1);
	pinChange(EN,0);
	//_delay_ms(1);
	_delay_us(100);
}

void Lcd4_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	Lcd4_Write_Char(a[i]);
}

void Lcd4_Shift_Right()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x0C);
}

void Lcd4_Shift_Left()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x08);
}
//End LCD 4 Bit Interfacing Functions
