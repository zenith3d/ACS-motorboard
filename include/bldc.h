#ifndef BLDC_H
#define BLDC_H

#include <stdlib.h>
#include "hall.h"
#include "m32m1_pwm.h"
//#include "timers.h"
//#include "uart.h"


// 6 possible phases from the hall effect sensors
#define         BLDC_PHASE_II           0b001
#define         BLDC_PHASE_IV           0b010
#define         BLDC_PHASE_III          0b011
#define         BLDC_PHASE_VI           0b100
#define         BLDC_PHASE_I            0b101
#define         BLDC_PHASE_V            0b110

// 6 possible transistor commutations
#define         BLDC_SET_Q2L_Q1H        0b001011
#define         BLDC_SET_Q3L_Q1H        0b100011
#define         BLDC_SET_Q3L_Q2H        0b101100
#define         BLDC_SET_Q1L_Q2H        0b001110
#define         BLDC_SET_Q1L_Q3H        0b110010
#define         BLDC_SET_Q2L_Q3H        0b111000





/*!
 * \brief bldc_init             Initilize BLDC motor (initialize PWM, and hall effect sensors)
 *                              The PWM frequency is given by the following parameters
 * \param prescaler             Value for setting the clock prescaler. Possible values are:
 *                                  PWM_PRESCALER_NONE
 *                                  PWM_PRESCALER_4
 *                                  PWM_PRESCALER_32
 *                                  PWM_PRESCALER_256
 *
 * \param sourceClock           Set the source clock for the PWM module. Possible values are:
 *                                  PWM_SOURCE_CLK_CPU_CLK
 *                                  PWM_SOURCE_CLK_PLL_32MHZ
 *                                  PWM_SOURCE_CLK_PLL_64MHZ
 *
 * \param deadTimeNumberCycles   Set the number of cycles for the dead-time. The duration of one cycle is given
 *                               by the period of the sourceClock divied by the prescaler. (One eleemntary cycle)
 */
void bldc_init(unsigned char prescaler=PWM_PRESCALER_NONE,
               unsigned char sourceClock=PWM_SOURCE_CLK_PLL_64MHZ,
               unsigned char deadTimeNumberCycles=PWM_DEADTIME_DEFAULT_NB_CYCLES);




/*!
 * \brief bldc_enableMotor  Enable motor (allows commutation)
 *                          Start the commutation process for controlling the motor
 */
void bldc_enableMotor();


/*!
 * \brief bldc_disableMotor Stop the commutation process of the motor
 *                          and open the six transistors of the H-Bridge
 */
void bldc_disableMotor();

/*!
 * \brief bldc_disableMotor Stop the commutation process of the motor and brakes the motor
 *                          braking is done by connecting the three phase to the ground
 *                          The three transistors connected to the ground are closed
 */
void bldc_brakeMotor();

/*!
 * \brief bldc_setSpeed Set the speed of the motor
 * \param Speed         Speed of the motor (PWM duty cycle)
 *                      Speed is included between -PWM_COUNTER_MAX and +PWM_COUNTER_MAX
 *                      Values outside of this range are bounded
 */
void bldc_setSpeed(int Speed);




/*!
 * \brief onInterruptHallChange This function is called when the hall sensor status has changed
 * \param Hall                  Current value of the sensor
 */
void onInterruptHallChange(uint8_t Hall);

/*!
 * \brief bldc_commutation  This function performs the righ commutation according to the current phase
 * \param Hall              Hall effect sensors : |0|0|0|0|0|H3|H2|H1|
 */
void bldc_commutation(uint8_t Hall);


#endif // BLDC_H
