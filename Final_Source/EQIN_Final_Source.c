/*
 * EQIN_Alarm_GY-61-FINAL.c
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
#define DISPOFF 0x08 //display off
#define DISPON 0x0c //display on
#define LINE2 0xc0 //2nd line move
#define HOME 0x02 //Cursor Home

//-----------------------------------------------------------

#define ADC1 0
#define ADC2 1
#define ADC3 2

#define X 0
#define Y 1
#define Z 2

#define IN_PLUS 5
#define EQ_PLUS 12
#define ERROR_CH 122

#define EQ_MAX 3 
#define IN_MAX 10
#define NOMAL 5

//-----------------------------------------------------------

int adc_number[3] = {0,};
int adc_number_char = 0;
int adc_flag = 0;

int avr_xyz[3] = {0,};
int avr_check = 0;
int avr_flag = 0;
	
int music[10] = {0x0777, 0x06A6, 0x05EC, 0x0597, 0x04FB, 0x0470, 0x03F4, 0x03BB, 0x0353};

int i = 0;
char write_word;
char write_test[256];

int judgment_flag = 0;
int in_save_flag = 0;
int error_flag = 0;

int eq_max_check = 0;
int in_max_check = 0;
int nomal_check = 0;

int in_persent = 0;
int in_save = 0;
int eq_i = 0;

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

void LCD_String(char * str)
{
	char *pstr =0;
	pstr=str;
	while(*pstr)
	DATA(*pstr++);
}

//-----------------------------------------------------------

ISR(INT0_vect)
{
	PORTC = 0x05;
	COMMAND(ALLCLR);
	LCD_String("Stop_System!");
	COMMAND(LINE2);
	for(i = 1;  i < 6; i++)
	{
		write_word = i + 48;
		DATA(write_word);
		DATA(' ');
		TCCR1A = 0xFE;
		ICR1 = music[1];
		OCR1A = ICR1 * 4 / 5;
		_delay_ms(100);
		TCCR1A = 0x02;
		_delay_ms(100);
	}
	LCD_String("OK!!");
	TCCR1A = 0xFE;
	ICR1 = music[2];
	OCR1A = ICR1 * 4 / 5;
	_delay_ms(500);
	TCCR1A = 0x02;
	
	COMMAND(ALLCLR);
	LCD_String("Start_System!");
	_delay_ms(500);
	COMMAND(LINE2);
	LCD_String("Now!");
	_delay_ms(1000);
	COMMAND(ALLCLR);
	
	PORTC = 0x02;
	EIFR = 0x01; //인터럽트 발생을 초기화 | 다시 인터럽트를 받을 수 있는 상태로 만듬 
	ADCSRA = 0b11001111;
}
	
//-----------------------------------------------------------

ISR(ADC_vect)
{
	if(adc_flag == 0)
	{
		adc_number[ADC1] = ADC;	
		ADMUX = 0b01000001;
	}
	
	else if(adc_flag == 1)
	{
		adc_number[ADC2] = ADC;
		ADMUX = 0b01000010;
	}
	
	else if(adc_flag == 2)
	{
		adc_number[ADC3] = ADC;
		ADMUX = 0b01000000;
	}
	
	if(adc_flag == 3)
	{
		if(avr_flag != 50 && avr_check == 0)
		{
			avr_xyz[X] += adc_number[ADC1];
			avr_xyz[Y] += adc_number[ADC2];
			avr_xyz[Z] += adc_number[ADC3];
			avr_flag++;	
		}
		
		else if(avr_flag == 50 && avr_check == 0)
		{
			avr_xyz[X] /= 50;
			avr_xyz[Y] /= 50;
			avr_xyz[Z] /= 50;
			avr_check = 1;
			avr_flag++;
		}
		/*
		adc_number_char = adc_number[ADC1];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);	
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = adc_number[ADC2];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = adc_number[ADC3];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		_delay_ms(100);
		COMMAND(ALLCLR);
		*/
		adc_flag = 0;
	}
	
	else
	{
		adc_flag++;
	}
}

//-----------------------------------------------------------

void control_value_analysis()
{
	int check_max_p = 0;
	
	if(avr_xyz[Z] - ERROR_CH >= adc_number[ADC3])
	{
		//COMMAND(ALLCLR);
		//LCD_String("SYSTEM_DOWN");
		judgment_flag = 3;
		in_persent = 0; 
		in_save_flag = 0;
		error_flag = 0;
		eq_i = 0;
		return;
	}


	
	if(avr_xyz[X] + IN_PLUS <= adc_number[ADC1] && avr_xyz[X] + EQ_PLUS > adc_number[ADC1])
	{
		//COMMAND(ALLCLR);
		//LCD_String("222");
		judgment_flag = 2;
		check_max_p = avr_xyz[X] - adc_number[ADC1];
		if(check_max_p < 0)
		{
			check_max_p = check_max_p * -1;
		}
	
		if(check_max_p > 10)
		{
			check_max_p = 10;
		}
	
		in_persent = 100 / (IN_PLUS * 2) * check_max_p;
		return;	
	}
	
	else if(avr_xyz[X] - IN_PLUS >= adc_number[ADC1] && avr_xyz[X] - EQ_PLUS < adc_number[ADC1])
	{
		//COMMAND(ALLCLR);
		//LCD_String("222");
		judgment_flag = 2;
		check_max_p = avr_xyz[X] - adc_number[ADC1];
		if(check_max_p < 0)
		{
			check_max_p = check_max_p * -1;
		}
		
		if(check_max_p > 10)
		{
			check_max_p = 10;
		}
		
		in_persent = 100 / (IN_PLUS * 2) * check_max_p;
		return;
	}
	
	else if(avr_xyz[Y] + IN_PLUS <= adc_number[ADC2] && avr_xyz[Y] + EQ_PLUS > adc_number[ADC2])
	{
		//COMMAND(ALLCLR);
		//LCD_String("222");
		judgment_flag = 2;
		check_max_p = avr_xyz[Y] - adc_number[ADC2]; 
		if(check_max_p < 0)
		{
			check_max_p = check_max_p * -1; 
		}
	
		if(check_max_p > 10)
		{
			check_max_p = 10;
		}
	
		in_persent = 100 / (IN_PLUS * 2) * check_max_p;
		return;	
	}
	
	else if(avr_xyz[Y] - IN_PLUS >= adc_number[ADC2] && avr_xyz[Y] - EQ_PLUS < adc_number[ADC2])
	{
		//COMMAND(ALLCLR);
		//LCD_String("222");
		judgment_flag = 2;
		check_max_p = avr_xyz[Y] - adc_number[ADC2];
		if(check_max_p < 0)
		{
			check_max_p = check_max_p * -1;
		}
	
		if(check_max_p > 10)
		{
			check_max_p = 10;
		}
	
		in_persent = 100 / (IN_PLUS * 2) * check_max_p;

		return;
	}
	
//-------------------------------------------------------------------------------/-/
	
	if(avr_xyz[X] + EQ_PLUS <= adc_number[ADC1])
	{
		//COMMAND(ALLCLR);
		//LCD_String("333");
		judgment_flag = 1;		
		return;
	}
	
	else if(avr_xyz[X] - EQ_PLUS >= adc_number[ADC1])
	{
		//COMMAND(ALLCLR);
		//LCD_String("333");
		judgment_flag = 1;
		return;
	}
	
	else if(avr_xyz[Y] + EQ_PLUS <= adc_number[ADC2])
	{
		//COMMAND(ALLCLR);
		//LCD_String("333");
		judgment_flag = 1;
		return;
	}
	
	else if(avr_xyz[Y] - EQ_PLUS >= adc_number[ADC2])
	{
		//COMMAND(ALLCLR);
		//LCD_String("333");		
		judgment_flag = 1;
		return;
	}
	
	//COMMAND(ALLCLR);
	//LCD_String("OK");
	
	check_max_p = avr_xyz[X] - adc_number[ADC1]; 
	if(check_max_p < 0)
	{
		check_max_p = check_max_p * -1; 
	}
	
	if(check_max_p > 10)
	{
		check_max_p = 10;
	}
	
	in_persent = 100 / (IN_PLUS * 2) * check_max_p;

	judgment_flag = 0;
	in_save_flag = 0;
	error_flag = 0;
	eq_i = 0;
	
}

//-----------------------------------------------------------

void led_control()
{		
		if(judgment_flag == 0)
		{
			PORTC = 0x02;
		}
		
		else if (judgment_flag == 1)
		{
			PORTC = 0x01;
		}
		
		else if (judgment_flag == 2)
		{
			PORTC = 0x04;
		}
		
		else
		{
			PORTC = 0x05;
		}
}

//-----------------------------------------------------------

void buzzer_control()
{
		if(judgment_flag == 0)
		{
			TCCR1A = 0x02;
		}
		
		else if (judgment_flag == 1)
		{
			TCCR1A = 0xFE;
			ICR1 = music[5];
			OCR1A = ICR1 * 4 / 5;
			_delay_ms(200);		
		}
		
		else if (judgment_flag == 2)
		{
			if(in_persent <= 50)
			{
				TCCR1A = 0xFE;
				ICR1 = music[4];
				OCR1A = ICR1 * 4 / 5;
				_delay_ms(400);
				TCCR1A = 0x02;
				_delay_ms(100);	
			}
			
			else if(in_persent >= 51 && in_persent <= 80 )
			{
				TCCR1A = 0xFE;
				ICR1 = music[4];
				OCR1A = ICR1 * 4 / 5;
				_delay_ms(300);
				TCCR1A = 0x02;
				_delay_ms(100);	
			}
			
			else
			{
				TCCR1A = 0xFE;
				ICR1 = music[4];
				OCR1A = ICR1 * 4 / 5;
				_delay_ms(200);
				TCCR1A = 0x02;
				_delay_ms(100);
			}			
		}
		
		else
		{
			TCCR1A = 0xFE;
			ICR1 = music[3];
			OCR1A = ICR1 * 4 / 5;
			_delay_ms(200);
		}
}

//-----------------------------------------------------------
//LCD Final

void lcd_control()
{
	char buf[100];	

	if(judgment_flag == 0)
	{
		COMMAND(ALLCLR);
		LCD_String("NOMAL_MODE!");
		COMMAND(LINE2);
		sprintf(buf,"%d%c",in_persent,'%');
		LCD_String(buf);
		_delay_ms(100);
	}
	
	else if(judgment_flag == 1)
	{
		if(eq_i == 0)
		{
			COMMAND(ALLCLR);
			LCD_String("Earth_Quake!!");
			for(eq_i = 0; eq_i < 5; eq_i++)
			{
				COMMAND(DISPOFF);
				_delay_ms(300);
				COMMAND(DISPON);
				_delay_ms(300);
			}
			COMMAND(ALLCLR);
			LCD_String("1. Hide under the Desk!");
			COMMAND(LINE2);
			LCD_String("2. stop that run to no building.");
			_delay_ms(500);
		}
		COMMAND(0x18);
		_delay_ms(300);
	}
	
	else if(judgment_flag == 2)
	{
		COMMAND(ALLCLR);
		if(in_save_flag == 0)
		{
			in_save = in_persent;
			in_save_flag = 1;
		}
		
		LCD_String("Interfloor_Noise!");
		COMMAND(LINE2);
		sprintf(buf,"%d%c",in_save,'%');
		LCD_String(buf);
	}
	
	else
	{
		if(error_flag == 0)
		{
			COMMAND(ALLCLR);
			error_flag = 1; 
		}
		LCD_String("Sensor_Error!");
		COMMAND(LINE2);
		LCD_String("Reset_System_Please");
	}
}

//-----------------------------------------------------------

int main(void)
{
		DDRE = 0xFF; //LCD모듈에 대한 핀설정
		PORTE = 0x00; //LCD모듈 핀 설정 초기화
	
		DDRC = 0x07; //3색 LED모듈에 대한 핀 설정 | 0x01 = RED, 0x02 = GREEN, 0x04 = BLUE
		PORTC = 0x02; //3색 LED모듈 GREEN색으로 초기화
		
		DDRB = 0xFF;
		
		TCCR1A = 0xFE;
		TCCR1B = 0x1A;
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		
		EIMSK = 0x01; //INT0의 인터럽트를 허용
		EICRA = 0x03; //인터럽트0의 하강에지 트리거
		
		
		LCD_INIT(); //LCD모듈 입력함수 접근
		
		sei(); //모든 인터럽트 활성화
		
//-----------------------------------------------------------		
		
		ADMUX = 0b01000001;
		ADCSRA = 0b11001111;
		
		while(1)
		{
			ADCSRA = 0b11001111;	
			if(avr_flag == 51)
			{
				break;
			}
		}
	
//-----------------------------------------------------------	
//AVR_XYZ-test
/*			
		adc_number_char = avr_xyz[X];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);	
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = avr_xyz[Y];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = avr_xyz[Z];
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = avr_flag;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		_delay_ms(1000);
		COMMAND(ALLCLR);
*/		
//-----------------------------------------------------------		
		
        while(1)
        {
			ADCSRA = 0b11001111;
			
			/*
			adc_number_char = adc_number[ADC1];
			write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
			DATA(write_word);
			write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
			DATA(write_word);
			write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
			DATA(write_word);
			DATA(' ');
			
			adc_number_char = adc_number[ADC2];
			write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
			DATA(write_word);
			write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
			DATA(write_word);
			write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
			DATA(write_word);
			DATA(' ');
			
			adc_number_char = adc_number[ADC3];
			write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
			DATA(write_word);
			write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
			DATA(write_word);
			write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
			DATA(write_word);
			DATA(' ');
			_delay_ms(50);
			COMMAND(ALLCLR);
			*/
			
			control_value_analysis();
			led_control();
			buzzer_control();
			lcd_control();
			
		}
		return 0;    
		
//-----------------------------------------------------------
		  
}