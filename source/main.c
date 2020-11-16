/*	Author: lab
 *  Partner(s) Name: Peter John Asence
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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

enum sound_states {note1, note2, note3, note4, note5, note6, wait1, wait2, wait3, end, realm} state;
int i = 0;
int flag = 1;
int tick = 0;
void tick_sound() {
	double notes[7] = {0,392.00, 587.33, 493.88 , 493.88, 440.00, 392.00};
	unsigned char but = 0x00;
	but = ~PINA & 0x07;
	
	switch(state){
		case realm:
			if (but == 0x01) state = note1;
			else  state = realm;
			break;
		case note1: 
			tick++;
			if (tick == 2) {
				state = note2;
				tick = 0;
			}
			else state = note1; //
			break;
		case note2:
			tick++;
			if( tick == 2) {
				state = note3;
				tick = 0;
			}
			else state = note2;
			break;
		case note3: 
			tick++;
			if( tick == 2){
			       	state = wait1;
				tick=0;
			}
			else state = note3;
			break;
		case wait1:
			tick++;
			if ( tick == 1) {
				state = note4;
				tick = 0;
			}
			else state = wait1;
			break;
		case note4: 
			tick++;
			if ( tick == 2) {
				state = wait2;
				tick = 0;
			}
			else  state = note4;
			break;
		case wait2:
			tick++;
			if (tick == 1){
				state = note5;
				tick = 0;
			}
			else state = wait2;
			break;
		case note5:
			tick++; 
			if (tick == 2){
				state = wait3;
				tick = 0;
			}
			else state = note5;
			break;
		case wait3:
			tick++;
			if (tick == 1){
				state = note6;
				tick = 0;
			}
			else state = note6;
			break;
		case end:
			break;
		default:
			state = realm;
			break;
	}
	
	switch(state){
		case realm:
			set_PWM(notes[0]);
			break;
		case note1:
			set_PWM(notes[1]);
			break;
		case note2:
			set_PWM(notes[2]);
			break;
		case note3:
			set_PWM(notes[3]);
			break;
		case wait1: 
			set_PWM(notes[0]);
			break;
		case note4:
			set_PWM(notes[4]);
			break;
		case wait2:
		      	set_PWM(notes[0]);
			break;
		case note5:
			set_PWM(notes[5]);
			break;
		case wait3:
			set_PWM(notes[0]);
			break;
		case note6:
			set_PWM(notes[7]);
			break;
		case end:
			PWM_off();
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

	TimerSet(100);
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
