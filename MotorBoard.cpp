#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "led.h"
#include "hall.h"
#include "pin.h"
#include "bldc.h"
#include "CanISR.h"



/*** USEFUL CONSTANTS ***/
#define PI2                 6.28319     //          The 2PI constant
#define ACCEL_REFRESH_HZ    100         // [Hz]     Board refresh frequency (using timer1 interrupts)
#define SPEED_REFRESH_HZ	20	        // [Hz]     The speed observer refresh rate
#define TICKS_TO_ROUNDS		48	        // [ticks]  Number of ticks for one round

/*** MOTOR CONSTANTS ***/
#define M_SPEEDCONST    	0.0330 	    // [V.s]    The motor speed constant used by the controller
#define M_VOLTAGE		    12	        // [V]      The supplied voltage

/*** PWM COMMAND ***/
#define VOLTS_TO_PWM_RATIO	2048/M_VOLTAGE	    //  The constant used to voltage to PWM value [-2048,2048]

/*** CAN IDs & MObs ***/
#define CAN_ID_SPEED  		0x30
#define CAN_ID_ACCEL		0x10



/** LEDS DECLARATION **/
LED red_led(&LED_RED_PORT, LED_RED_PIN, LED_RED_POL);
LED yellow_led(&LED_YELLOW_PORT, LED_YELLOW_PIN, LED_YELLOW_POL);

/** GLOBAL VARIABLES **/
int16_t voltage_cmd_pwm;    // [-2048, 2048] PWM value to encode the voltage : [-M_VOLTAGE, M_VOLTAGE]
float accel_cmd_radss;	    // [rad.s-2]    The acceleration command (received from the CAN bus)
float speed_cmd_rads;	    // [rad.s-1]    The speed command (refreshed every timer1 int. to achieve the accel cmd)
float speed_rads;	        // [rad.s-1]    The actual motor speed
int64_t position;	        // [ticks]	    The actual motor position

uint8_t can_buff[8];	    // The CAN buffer used to send data
uint8_t accel_buff[8];	    // The CAN buffer received to set acceleration



int main(void) {
    cli();

    // Global variables initialization
    voltage_cmd_pwm = 0.;
    accel_cmd_radss = 0.;
    speed_cmd_rads = 0.;
    speed_rads = 0.;
    position = 0;
    
    // Enable the LM2575 (step-down switching voltage regulator)
    Output LM2575_enable(&PORTC, PC7);
    LM2575_enable.setLow();

    // Timer1 initialization
    TCCR1A = 0x00;				                // Set CTC mode 
    TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);	// Set CTC mode and Prescaler set to 64 : 250kHz
    TIMSK1 = (1<<OCIE1A); 			            // Enable compare interrupt on A
    OCR1A  = 2500;     				            // 250kHz/2500 = 100Hz

    // CAN Bus initialization (500Kb/s)
    initCANBus();
    initCANMOBasReceiver(1, CAN_ID_ACCEL, 0);

    // BLDC Motor initialization
    bldc_init();
    bldc_enableMotor();
	
    // Blink the leds
    for(int i=0; i<4; i++){
        red_led.blink(50);
        yellow_led.blink(50);
    }    

    sei();
    
    while(1) {
        // Compute the motor speed
        speed_rads = (float)((hall_getPosition() - position)*PI2*SPEED_REFRESH_HZ/TICKS_TO_ROUNDS);  

        // Send data on CAN BUS
        memcpy(&can_buff[0], &(speed_rads), sizeof(float));
        sendData(0, CAN_ID_SPEED, 4, can_buff);
	
        position = hall_getPosition();
        _delay_ms(50);      // 20Hz
    }
}


ISR(TIMER1_COMPA_vect) {
    /**
     * Interrupt handling the computation of the voltage needed to be sent to the motor according 
     * to the desired torque command.
    **/
    cli();
	red_led.on();  

    // Numerically integrate the desired acceleration
    speed_cmd_rads = speed_cmd_rads + accel_cmd_radss / ACCEL_REFRESH_HZ;

    // Compute the voltage command
    voltage_cmd_pwm = (int16_t) (VOLTS_TO_PWM_RATIO * M_SPEEDCONST * speed_cmd_rads);
    bldc_setSpeed(voltage_cmd_pwm);

    red_led.off();
    sei();
}


ISR(CAN_INT_vect) {
    cli();

    if ((CANSIT2 & 0x02) != 0x00) {			// SET ACCEL REQUEST
        yellow_led.on();
        CANPAGE = 0x10; uint8_t dlc = CANCDMOB & 0x0F;	// Select MOb1 - Get the DLC of the CAN frame
	    if(dlc == 0x04) {
            CANPAGE = 0x10; accel_buff[0] = CANMSG;
            CANPAGE = 0x11; accel_buff[1] = CANMSG;
            CANPAGE = 0x12; accel_buff[2] = CANMSG;
            CANPAGE = 0x13; accel_buff[3] = CANMSG;
	        memcpy(&accel_cmd_radss, accel_buff, sizeof(float));
	    }
        // Reset the MOB configuration for next CAN message
        CANPAGE   = 0x10; // select MOB1
        CANSTMOB  = 0x00; // Reset the status of the MOB1
        CANCDMOB  = 0x80; // Config as reception MOB1
        CANIE2   |= 0x02; // Enable the interruption over MOB 1 (for the next one)
        CANSIT2  &= 0xFD; // remove the MOB1 raised flag
        yellow_led.off();
    }

    sei();
}
