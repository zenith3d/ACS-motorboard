/*
 * config.h
 
	Config MotorBoard
	Auteur: F.Mercier
	Date: 01/12/2016
 
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

//_____ I N C L U D E S ________________________________________________________
#include <avr/io.h>
#include <avr/interrupt.h>

//_____ M A C R O S ____________________________________________________________
typedef uint8_t Bool;

//_____ D E F I N I T I O N S __________________________________________________

/*********************************/
/*    MCU LIB CONFIGURATION      */
/*********************************/

#define FOSC           	16000
#define F_CPU          	16000000UL

/*********************************/
/*    CAN LIB CONFIGURATION      */
/*********************************/

#define CAN_BAUDRATE   	500		// [Kb/s]

/************************/
/*    UART              */
/************************/

#define	UART_BITRATE 	1		
#define	UART_BAUDRATE   115200

/************************/
/*    LED               */
/************************/

#define LED_RED_PORT    PORTB
#define LED_RED_PIN     3
#define LED_RED_POL     0

#define LED_YELLOW_PORT PORTB
#define LED_YELLOW_PIN  2
#define LED_YELLOW_POL  0

//_____ D E C L A R A T I O N S ________________________________________________

#endif  // _CONFIG_H_
