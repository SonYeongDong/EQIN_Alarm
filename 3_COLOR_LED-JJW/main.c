/*
 * team_project.c
 *
 * Created: 2018-12-03 오후 2:49:21
 * Author : dsm2017
 */ 

#include <avr/io.h>

int led_control(void){
	DDRC = 0xff;
	int judgment_flag = 0;
	
	if(judgment_flag = 0){
		PORTC = 0x01;
	}
	else if (judgment_flag = 1)
	{
		PORTC =0x02;
	}
	else if (judgment_flag = 2)
	{
		PORTC =0x04;
	}
	else {
		PORTC = 0x05;
	}
	
}
int main(void)
{
	led_control();
    /* Replace with your application code */
    while (1) 
    {
    }
}

