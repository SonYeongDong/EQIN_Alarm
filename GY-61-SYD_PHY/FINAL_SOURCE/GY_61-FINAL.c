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
#define DISPON 0x0c //display on
#define LINE2 0xc0 //2nd line move
#define HOME 0x02 //Cursor Home

//-----------------------------------------------------------

int adc_number1 = 0;
int adc_number2 = 0;
int adc_number3 = 0;
int adc_number_char = 0;
int adc_flag = 0;

int music[10] = {0x0777, 0x06A6, 0x05EC, 0x0597, 0x04FB, 0x0470, 0x03F4, 0x03BB, 0x0353};

int i = 0;
char write_word;
char write_test[256];

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
/*
void LED(void)
{
	if(final_val > 0 && button_flag == 0)
	{
		PORTC = 0x02; //일반모드
	}
	
	else if(final_val < -200)
	{
		PORTC = 0x01; //녹음모드
	}
	
	else if(button_flag == 1)
	{
		PORTC = 0x04;
		button_flag = 0;
	}
}
*/
//-----------------------------------------------------------

/*
ISR(INT0_vect)
{
	final_val = pulse_count; //final_val에 현재 pulse_count값 저장 
	pulse_count = 0; //pulse_count 초기화   
	EIFR = 0x01; //인터럽트 발생을 초기화 | 다시 인터럽트를 받을 수 있는 상태로 만듬 
	ADCSRA = 0b11001111;
	
	
	sprintf(write_test, "%d|", final_val);
	for(i = 0; i < 4; i++)
	{
		DATA(write_test[i]);
	}
	
}
*/

	
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

		adc_number_char = adc_number1;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);	
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = adc_number2;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		
		adc_number_char = adc_number3;
		write_word = (adc_number_char - adc_number_char/1000*1000)/100 + 48;
		DATA(write_word);
		write_word = ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10 + 48;
		DATA(write_word);
		write_word = (((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100) - ((adc_number_char - adc_number_char/1000*1000) - (adc_number_char - adc_number_char/1000*1000)/100*100)/10*10)/1 + 48;
		DATA(write_word);
		DATA(' ');
		_delay_ms(100);
		COMMAND(ALLCLR);
		adc_flag = 0;
		
	}
	else
	{
		adc_flag++;
	}
}

//-----------------------------------------------------------
/*
void sound_out(void)
{
	if(adc_number1 > 200)
	{
		ICR1 = music[2];
		OCR1A = ICR1 * 4 / 5;
	}
	
	else if(adc_number2 > 200)
	{
		ICR1 = music[1];
		OCR1A = ICR1 * 4 / 5;
	}
	
	else if(adc_number3 > 200)
	{
		ICR1 = music[0];
		OCR1A = ICR1 * 4 / 5;
	}
		
	else 
	{
		ICR1 = NOS;
		OCR1A = NOS + 2;
	}
}
*/
//-----------------------------------------------------------
int main(void)
{
		/*
		DDRC = 0x07; //3색 LED모듈에 대한 핀 설정 | 0x01 = RED, 0x02 = GREEN, 0x04 = BLUE
		PORTC = 0x02; //3색 LED모듈 GREEN색으로 초기화
		*/
		
		DDRE = 0xFF; //LCD모듈에 대한 핀설정
		PORTE = 0x00; //LCD모듈 핀 설정 초기화
	
		//DDRB = 0xFF;
		/*
		TCCR1A = 0xFE;
		TCCR1B = 0x1A;
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		*/
		
		/*
		EIMSK = 0x01; //INT0의 인터럽트를 허용
		EICRA = 0x03; //인터럽트0의 하강에지 트리거
		*/
		
		LCD_INIT(); //LCD모듈 입력함수 접근
		
		sei(); //모든 인터럽트 활성화
		
        while(1)
        {
			ADCSRA = 0b11001111;
			/*
                pulse(); //pulse함수 호출
				LED();
				sound_out();
			*/
		}      
}