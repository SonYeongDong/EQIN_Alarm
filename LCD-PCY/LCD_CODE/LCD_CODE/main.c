/*
 * LCD_CODE.c
 *
 * Created: 2018-11-26 오후 3:21:15
 * Author : PCY
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

#define FUNCSET 0x28
#define ENTMODE 0x06
#define ALLCLR 0x01
#define DISPON 0x0c
#define LINE 0xc0

void COMMAND(unsigned char byte)
{
	_delay_ms(2);
	
	PORTC = byte & 0xf0;
	PORTC &= 0b11111100;
	
	_delay_us(1);
	PORTC |= 0b00000100;
	_delay_us(1);
	PORTC &= 0b11111011;
	PORTC = (byte << 4) & 0xf0;
	PORTC &= 0b11111100;
	_delay_us(1);
	PORTC |= 0b00000100;
	_delay_us(1);
	PORTC &= 0b11111011;
}

void DATA(unsigned char byte)
{
	_delay_ms(2);
	
	PORTC = byte & 0xf0;
	
	PORTC |= 0b00000001;
	PORTC &= 0b11111101;
	_delay_us(1);
	PORTC |= 0b00000100;
	_delay_us(1);
	PORTC &= 0b11111011;
	
	PORTC = (byte << 4) & 0xf0;
	
	PORTC |= 0b00000001;
	
	PORTC &= 0b11111101;
	
	_delay_us(1);
	PORTC |= 0b00000100;
	_delay_us(1);
	PORTC &= 0b11111011;
}

void LCD_INIT(void)
{
	_delay_ms(30);
	
	COMMAND(0b00101000);
	_delay_us(39);
	
	COMMAND(0b00001100);
	_delay_us(39);
	
	COMMAND(0b00000001);
	_delay_ms(1.53);
	COMMAND(0b00000110);
}

void LCD_String(char * str)
{
	char *pstr =0;
	pstr=str;
	while(*pstr)
	DATA(*pstr++);
}

void busy(void)
{
	_delay_ms(2);
}

int in_persent = 60;
int judgment_flag = 1;

void lcd_control()
{
	DDRC = 0xff;
	int in_temp;
	char buf[100];
	
	LCD_INIT();
	if(judgment_flag == 0){
		if(in_persent>=0 && in_persent < 10)
		{
			sprintf(buf,"%s%d%c","NOMAL         ",in_persent,'%');
			LCD_String(buf);
		}
		else
		{
			sprintf(buf,"%s%d%c","NOMAL        ",in_persent,'%');
			LCD_String(buf);
			
		}
	}
	else if(judgment_flag == 1)
	{
		LCD_String("Hide under the table.  Ane Then When an earthquake stops, protect your head and evacuate to a place where there are no buildings.  ");
		COMMAND(0x18);
		_delay_ms(1000);

	}
	else
	{
		LCD_String("interfloor noise!");
		COMMAND(LINE);
		sprintf(buf,"%d%c",in_persent,'%');
		LCD_String(buf);
	}
}
int main(void)
{

    while (1) 
    {
		lcd_control();
    }
}

