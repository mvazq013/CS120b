/*	Name & Email: Marco Vazquez, mvazq013@ucr.edu
 *	Lab Section: 021
 *	Assignment: Final Project Remote Control Car
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "usart1284.h"

enum Sensor{OFF, ON}sensor_state;
enum Bluetooth{START, RECEIVE} bt_state;
	
unsigned char sensor = 0x00;
unsigned char motor = 0x00;
unsigned char Data_in = 0x00;
	
void BluetoothTick() {
	switch(bt_state){				// STATE TRANSITIONS
		case START:
			if (!USART_HasReceived(0)) {
				bt_state = START;
			}
			else {
				bt_state = RECEIVE;
			}
			break;

		case RECEIVE:
			bt_state = START;
			break;
	}								// STATE TRANSITIONS
	
	switch(bt_state){				// STATE ACTIONS
		case START:
			break;

		case RECEIVE:
		Data_in = USART_Receive(0);
		
			if (Data_in == '1') {				//STATEMENTS USED TO CONTROL WHEELS/DIRECTION OF CAR BASED ON INPUT FROM CONTROLLER/PHONE
				motor = (motor & 0xF0) | 0x05;	
			}
			else if (Data_in == '2') {
				motor = (motor & 0xF0) | 0x06;	
			}
			else if (Data_in == '3') {
				motor = (motor & 0xF0) | 0x00;
			}
			else if (Data_in == '4') {
				motor = (motor & 0xF0) | 0x09;
			}
			else if (Data_in == '5') {
				motor = (motor & 0xF0) | 0x0A;
			}
			USART_Send(0x01, 0);
			
			break;
	}								// STATE ACTIONS
}

void SensorTick() {
	switch(sensor_state) {			// STATE TRANSITIONS
		case OFF:
			if (sensor == 0x01) {
				sensor_state = ON;
			}
			else if (sensor == 0x00) {
				sensor_state = OFF;
			}
			break;
		case ON:
			if (sensor == 0x01) {
				sensor_state = ON;
			}
			else if (sensor == 0x00) {
				sensor_state = OFF;
			}
			break;
		default:
			break;
		}								// STATE TRANSITIONS
		
	switch(sensor_state) {				// STATE ACTIONS
		case OFF:
			break;
		case ON:
			motor = (motor & 0xF0) | 0x00;	// STOPS CAR WHEN SENSOR COMES CLOSE TO OBJECT
			break;
	}									// STATE ACTIONS
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;	// Configure B's 8 pins as outputs. Initialize to 0s (output motor to PORTB)
    DDRC = 0x00; PORTC = 0xFF;	// Configure C's 8 pins as inputs. Initialize to 1s (sensor used as input)
	
	initUSART(0);
	
	sensor_state = OFF;
	
    while (1) {
		sensor = ~PINC & 0x01;
		
		SensorTick();
		BluetoothTick();
		
		PORTB = motor;
    }
}