/*
 * EQIN_Alarm_GY_61-TEST_XYZ.c
 *
 * Created: 2018-11-23 오후 11:02:37
 * Author : SYD_PHY
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

//-----------------------------------------------------------

#define FUNCSET 0x28 //function set
#define ENTMODE 0x06 //entry mode set
#define ALLCLR 0x01 //all clear
#define DISPON 0x0c //display on
#define LINE2 0xc0 //2nd line move
#define HOME 0x02 //Cursor Home

//-----------------------------------------------------------

int i = 0;

int adc_number1 = 0;
int adc_number2 = 0;
int adc_number3 = 0;
int adc_number_char = 0;
int adc_flag = 0;

int max_x = 0;
int max_y = 0;
int max_z = 0;

int min_x = 0;
int min_y = 0;
int min_z = 0;
int mm_flag = 0;

char write_word;
char xyz_test[256] = "shake_board!";
char test_result[256] = "test_complite!";

//-----------------------------------------------------------

void COMMAND(unsigned char byte)
{
	_delay_ms(2);
	
	PORTE = byte & 0xf0;
	PORTE &= 0b11111100;
	
	_delay_us(1);
	PORTE |= 0b00000100;
	_delay_us(1);
	PORTE &= 0b11111011;
	PORTE =(byte<<4) & 0xf0;
	PORTE &= 0b11111100;
	_delay_us(1);
	PORTE |= 0b00000100;
	_delay_us(1);
	PORTE &= 0b11111011;
}

void DATA(unsigned char byte)
{
	_delay_ms(2);
	
	PORTE = byte & 0xf0;
	
	PORTE |= 0b00000001;
	PORTE &= 0b11111101;
	_delay_us(1);
	PORTE |= 0b00000100;
	_delay_us(1);
	PORTE &= 0b11111011;
	
	PORTE =(byte<<4) & 0xf0;
	PORTE |= 0b00000001;
	PORTE &= 0b11111101;
	_delay_us(1);
	PORTE |= 0b00000100;
	_delay_us(1);
	PORTE &= 0b11111011;
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

//-----------------------------------------------------------

ISR(ADC_vect)
{
	if(adc_flag == 0)
	{
		adc_number1 = ADC;	
		ADMUX = 0b01000001;
	}
	
	else if(adc_flag == 1)
	{
		adc_number2 = ADC;
		ADMUX = 0b01000010;
	}
	
	else if(adc_flag == 2)
	{
		adc_number3 = ADC;
		ADMUX = 0b01000000;
	}	
	
	if(adc_flag == 3)
	{
		if(max_x < adc_number1)
		{
			max_x = adc_number1;
		}
		
		if(max_y < adc_number2)
		{
			max_y = adc_number2;
		}
		
		if(max_z < adc_number3)
		{
			max_z = adc_number3;
		}
		
		
		if(adc_number1 < 340)
		{
			min_x = adc_number1;
		}
		
		if(adc_number2 < 344)
		{
			min_y = adc_number2;
		}
		
		if(adc_number3 < 412)
		{
			min_z = adc_number3;
		}
		
		mm_flag++;
		adc_flag = 0;
	}
	else
	{
		adc_flag++;
	}
}

//-----------------------------------------------------------

int main(void)
{		
		DDRE = 0xFF; //LCD모듈에 대한 핀설정
		PORTE = 0x00; //LCD모듈 핀 설정 초기화
			
		LCD_INIT(); //LCD모듈 입력함수 접근
		
		COMMAND(ALLCLR);
		for(i = 0; i < 12; i++)
		{
			DATA(xyz_test[i]);	
		}
		
		sei(); //모든 인터럽트 활성화
		
        while(1)
        {
			ADCSRA = 0b11001111;
			
			if(mm_flag == 10000)
			{
				break;
			}
		}      

//-----------------------------------------------------------

		COMMAND(ALLCLR);
		
		DATA('m');
		DATA(':');
		adc_number_char = max_x;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);	
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = max_y;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = max_z;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		COMMAND(LINE2);

//----------------------------------------------------------

		DATA('n');
		DATA(':');
		adc_number_char = min_x;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = min_y;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = min_z;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		return 0;
}