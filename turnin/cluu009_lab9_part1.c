/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/YYJeFRz0TyY
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

enum States{START, INIT, CNOTE, DNOTE, ENOTE} state;

void Tick(){
    switch(state){
        case START:
            state = INIT;
            break;
        case INIT:
            if((~PINA & 0x07) == 0x01){
                state = CNOTE;
            }
            else if((~PINA & 0x07) == 0x02){
                state = DNOTE;
            }
            else if((~PINA & 0x07) == 0x04){
                state = ENOTE;
            }
            else{
                state = INIT;
            }
            break;
        case CNOTE:
            if((~PINA & 0x07) == 0x01){
                state = CNOTE;
            }
            else{
                state = INIT;
            }
            break;
        case DNOTE:
            if((~PINA & 0x07) == 0x02){
                state = DNOTE;
            }
            else{
                state = INIT;
            }
            break;
        case ENOTE:
            if((~PINA & 0x07) == 0x04){
                state = ENOTE;
            }
            else{
                state = INIT;
            }
            break;
        default:
            state = INIT;
            break;
    }
    switch(state){
        case START:
            break;
        case INIT:
            set_PWM(0);
            break;
        case CNOTE:
            set_PWM(261.63);
            break;
        case DNOTE:
            set_PWM(293.66);
            break;
        case ENOTE:
            set_PWM(329.63);
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
    while (1) {
        Tick();
    }
    return 1;
}
