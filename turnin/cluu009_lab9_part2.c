/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/uc_EuQZQRN8 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
    static double current_frequency;
    if(frequency != current_frequency){
        if(!frequency){
            TCCR3B &= 0x08;
        }
        else{
            TCCR3B |= 0x03;
        }

        if(frequency < 0.954){
            OCR3A = 0xFFFF;
        }
        else if(frequency > 31250){
            OCR3A = 0x0000;
        }
        else{
            OCR3A = (short)(8000000 / (128 * frequency)) - 1;
        }
        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on(){
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off(){
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum States{START, INIT, ON, INC, DEC, RELEASE} state;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned int index = 0;
unsigned int onoff = 0;

void Tick(){
    switch(state) {
		case START:	
            state = INIT; 
            break;
		
		case INIT:	
            if ((~PINA & 0x07) == 0x01){
                state = ON;
            }
			else if ((~PINA & 0x07) == 0x02){
                state = INC;
            }
			else if ((~PINA & 0x07) == 0x04) {
                state = DEC;
            }
			else {
                state = INIT;
            }
			break;
		case INC:	
            state = RELEASE;
			break;
		case DEC:	
            state = RELEASE;
            break;
		case ON:	
            state = RELEASE;
			break;
		case RELEASE:	
            if ((~PINA & 0x07) == 0x00){
                state = INIT;
            }
            else {state = RELEASE;
            }
            break;
		default:	
            state = START; 
            break;
	}
	switch (state) {
		case START:     
            break;
		case INIT:	
            break;
		case INC:	
            if (index < 7){
                ++index;
            } 
			if (onoff == 1){
                set_PWM(notes[index]);
            }
			break;
		case DEC:	
            if (index > 0){
                --index;
            } 
			if (onoff == 1){
                set_PWM(notes[index]);
            }
			break;
		case ON:	
            if (onoff == 0){
                onoff = 1; 
                set_PWM(notes[index]);
            }
			else {
                onoff = 0; 
                set_PWM(0);
            }
			break;
		case RELEASE:	
            break;
        default:       
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
    PWM_on();
    state = START;
    index = 0;
    onoff = 0;
    while (1) {
        Tick();
    }
    return 1;
}
