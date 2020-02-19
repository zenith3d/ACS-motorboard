#ifndef HALL_H
#define HALL_H

#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/io.h>
#include <stdint.h>


#define             HALL_DIRECTION_CW       1
#define             HALL_DIRECTION_CCW      0



#define             HALL_1_PORT             PORTD
#define             HALL_1_DDR              DDRD
#define             HALL_1_PIN              PIND
#define             HALL_1_BIT              PD7
#define             HALL_1_PCMSK            PCMSK2
#define             HALL_1_PCINT            PCINT23


#define             HALL_2_PORT             PORTD
#define             HALL_2_DDR              DDRD
#define             HALL_2_PIN              PIND
#define             HALL_2_BIT              PD5
#define             HALL_2_PCMSK            PCMSK2
#define             HALL_2_PCINT            PCINT21


#define             HALL_3_PORT             PORTC
#define             HALL_3_DDR              DDRC
#define             HALL_3_PIN              PINC
#define             HALL_3_BIT              PC6
#define             HALL_3_PCMSK            PCMSK1
#define             HALL_3_PCINT            PCINT14



// _______________________
// ::: Initializations :::


/*!
* \brief hall_init Initialize hall sensor
*                  Set port connected to H1, H2 and H3 as input
*                  Allows interruption on pin change (PCINT23=H1 PCINT14=H2 PCINT21=H3)
*                  Note that sei is called in this function
*/
void hall_init();






// _________________
// ::: Interrupt :::



/*!
 * \brief hall_attachInterrupt  Attach an interrupt to the pin change interrupt
 * \param ptrFunction           This function is called every change on the hall sensors
 *                              void ptrFunction(unsigned char HallSensorStatus)
 */
void hall_attachInterrupt( void (*ptrfonction)(unsigned char));



/*!
 * \brief hall_detachInterrupt  Detach the current function from the interrupt
 *                              see hall_attachInterrupt for more details
 */
void hall_detachInterrupt();


/*!
 * \brief ISR(PCINT1_vect)  Interrupt Service Routine
 *                          called on changes on sensor H2
 *                          Read hall sensors, call user function if attached
 *                          Update direction and position
 */
ISR(PCINT1_vect);


/*!
 * \brief ISR(PCINT2_vect)  Interrupt Service Routine
 *                          called on changes on sensors H1 or H3
 *                          Read hall sensors, call user function if attached
 *                          Update direction and position
 */
ISR(PCINT2_vect);


// ___________________________
// ::: Getters and setters :::



/*!
 * \brief hall_getSensors   get hall sensors
 * \return                  return the combination of the 3 hall sensors
 *                            7   6   5   4   3   2    1    0
 *                          | 0 | 0 | 0 | 0 | 0 | H3 | H2 | H1 |
 */
uint8_t         hall_getSensors();

/*!
 * \brief hall_getDirection getter on direction of rotation (0=CW 1=CCW)
 * \return                  false if the rotation is clockwise
 *                          true if the rotation is counter clockwise
 */
bool            hall_getDirection();

/*!
 * \brief hall_getPosition  getter on the current position
 *                          The position is given in term on hall sensor changes
 * \return                  the current position of the motor
 */
int64_t         hall_getPosition();

/*!
 * \brief hall_setPosition  set the current position of the motor
 * \param Position          new current position
 */
void            hall_setPosition(int64_t Position);


/*!
 * \brief hall_getError getter on errors
 * \return              false = no error
 *                      true means that an error occured, the new sensor data disagree with previous one
 *                      this happens when the rotation speed is faster than interrupts maximum frequency
 *
  */
bool            hall_getError();

/*!
 * \brief hall_resetError   reset the hall sensor error (hall_ErrorHallSensors is unset)
 */
void            hall_resetError();





// __________________________
// ::: Internal functions :::

/*!
 * \brief hall_onChange This is call everytime a change occured on a sensor
 *                      compute the direction of rotation and update the position
 *                      if the new value disagree with previous one, hall_ErrorHallSensors
 *                      is set to one.
 * \param currentStatus Current status of the hall sensors ( |0|0|0|0|0|H3|H2|h1|)
 */
void            hall_onChange(uint8_t currentStatus);







#endif // HALL_H
