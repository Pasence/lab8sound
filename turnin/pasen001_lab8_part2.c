/*	Author: lab
 *  Partner(s) Name: Peter John Asence
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/FfM7YaIkv0c
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer stuff
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr =0;

void TimerOn(){
        TCCR1B = 0x0B;
        OCR1A = 123;
        TIMSK1 = 0x02;
        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |=0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag=1;
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0) {
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}
void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}
//

// sound functions
void set_PWM(double frequency) {
	static double current_frequency;
	
	if (frequency != current_frequency) { 
		if (!frequency) {TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; } 

		if (frequency < 0.954) {OCR3A = 0xFFFF; }

		else if (frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short)( 8000000 / (128 *frequency) - 1); }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

//

enum sound_states {button1_p, button1_r,button2_p, button2_r, button3_p, button3_r, realm} state;
int i = 0;
int flag = 1;
void tick_sound() {
	double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
	unsigned char but = 0x00;
	but = ~PINA & 0x07;
	
	switch(state){
		case realm:
			if (but == 0x01) state = button1_p;
		        else if (but == 0x02) state = button2_p;
			else if (but == 0x04) state = button3_p;
			else  state = realm;
			break;
		case button1_p: 
			if (but == 0x00) state = button1_r;
			else state = button1_p; //
			break;
		case button1_r:
			state = realm;
			break;
		case button2_p: 
			if( but == 0x00) state = button2_r;
			else state = button2_p;
			break;
		case button2_r:
			state = realm;
			break;
		case button3_p: 
			if ( but == 0x00) state = button3_r;
			else  state = button3_p;
			break;
		case button3_r:
			state = realm;
			break;
		default:
			state = realm;
			break;
	}
	
	switch(state){
		case realm:
			set_PWM(notes[i]);
			break;
		case button1_p:
			break;
		case button1_r:
			if (i < 7){
		        i += 1;
			set_PWM(notes[i]); }
			break;
		case button2_p:
			break;
		case button2_r: 
			if ( i > 0){
			i -= 1;
			set_PWM(notes[i]);}
			break;
		case button3_p:
			break;
		case button3_r:
		      if( flag == 1){
		      	PWM_off() ;
			flag = 0;
			}
		       else if(flag == 0) {
			       PWM_on();
			       flag = 1;
		       }
			break;
		default: 
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
    /* Insert your solution below */

	TimerSet(500);
	TimerOn();
	PWM_on();
	state = realm;
    while (1) {
	tick_sound();
	while(!TimerFlag)
	TimerFlag = 0;
    }
    return 1;
}
