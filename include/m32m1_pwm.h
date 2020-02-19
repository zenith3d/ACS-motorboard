#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <stdint.h>
#include "m32m1_pll.h"


#define	    PSCOUT0A_PORT   PORTD
#define	    PSCOUT0A_PIN    0
#define	    PSCOUT0B_PORT   PORTB
#define	    PSCOUT0B_PIN    7

#define	    PSCOUT1A_PORT   PORTC
#define	    PSCOUT1A_PIN    0
#define	    PSCOUT1B_PORT   PORTB
#define	    PSCOUT1B_PIN    6

#define	    PSCOUT2A_PORT   PORTB
#define	    PSCOUT2A_PIN    0
#define	    PSCOUT2B_PORT   PORTB
#define	    PSCOUT2B_PIN    1


// Define possible input clock (CPU clock, PLL 64MHz or PLL 32MHz)
#define         PWM_SOURCE_CLK_CPU_CLK          1
#define         PWM_SOURCE_CLK_PLL_32MHZ        2
#define         PWM_SOURCE_CLK_PLL_64MHZ        3


// Prescaler settings
#define         PWM_PRESCALER_NONE              0b00000000
#define         PWM_PRESCALER_4                 0b01000000
#define         PWM_PRESCALER_32                0b10000000
#define         PWM_PRESCALER_256               0b11000000


// Default number of cycles for dead-time
#define         PWM_DEADTIME_DEFAULT_NB_CYCLES  8


// Maximum value for the PSC counter (maximum value is 4096 - dead time)
#define         PWM_COUNTER_MAX_DEFAULT         2048


// When the function pwm_setOutputConfiguration is called with this parameter,
// all the PWM are disable.
#define         PWM_CONFIG_DISABLE_ALL          0b000000




class m32m1_pwm
{
public:

    /*!
     * \brief m32m1_pwm		    Constructor, do nothing
     *				    Note that ports must be configured as output before using the PWM
     */
    m32m1_pwm();


    /*!
     * \brief init		    Initialize PWM at a given frequency.
     *                              This frequency is set by the parameters
     *
     * \param prescaler             Value for setting the clock prescaler. Possible values are:
     *				    PWM_PRESCALER_NONE	=> No divider on PSC input clock
     *				    PWM_PRESCALER_4	=> Divide the PSC input clock by 4
     *				    PWM_PRESCALER_32	=> Divide the PSC input clock by 32
     *				    PWM_PRESCALER_256	=> Divide the PSC clock by 256
     *
     * \param sourceClock           Set the source clock for the PWM module. Possible values are:
     *				    PWM_SOURCE_CLK_CPU_CLK	=> select the slow clock input (CLKIO).
     *				    PWM_SOURCE_CLK_PLL_32MHZ	=> select the fast clock input (CLKPLL) at 32MHz.
     *				    PWM_SOURCE_CLK_PLL_64MHZ	=> select the fast clock input (CLKPLL) at 64MHz.
     *
     * \param deadTimeNumberCycles  Set the number of cycles for the dead-time. The duration of one cycle is given
     *                              by the period of the sourceClock divied by the prescaler. (One eleemntary cycle)
     *
     * \param counterMaximum	    Maximum value of the counter (output compare match)
     */
    void    init   (uint8_t	prescaler,
		    uint8_t	sourceClock,
		    uint8_t	deadTimeNumberCycles,
		    uint16_t    counterMaximum=PWM_COUNTER_MAX_DEFAULT);


    /*!
     * \brief setPrescaler	    Set prescaler (select the PSC input clock division factor)
     * \param prescaler		    requested prescaler, possible values are:
     *				    PWM_PRESCALER_NONE	=> No divider on PSC input clock
     *				    PWM_PRESCALER_4	=> Divide the PSC input clock by 4
     *				    PWM_PRESCALER_32	=> Divide the PSC input clock by 32
     *				    PWM_PRESCALER_256	=> Divide the PSC clock by 256
     */
    void    setPrescaler(uint8_t prescaler);


    /*!
     * \brief setSourceClock	    Set the source clok for the PWM device
     * \param sourceClock	    requested source clock, possible values are:
     *				    PWM_SOURCE_CLK_CPU_CLK	=> select the slow clock input (CLKIO).
     *				    PWM_SOURCE_CLK_PLL_32MHZ	=> select the fast clock input (CLKPLL) at 32MHz.
     *				    PWM_SOURCE_CLK_PLL_64MHZ	=> select the fast clock input (CLKPLL) at 64MHz.
     */
    void	setSourceClock(uint8_t sourceClock);


    /*!
     * \brief setCounterMax	    Set the maximum value fo the PWM counter (output compare match)
     * \param counterMaximum	    Maximum value, this is also equal to the duty-cycle maximum
     *				    The maximum value is 4096 - number of cycle for deadtime
     */
    void	setCounterMax(uint16_t counterMaximum);


    /*!
     * \brief setOutputConfiguration	Enable or disable each PWM channel
     * \param Config                    This variable is composed of 6 bits :
     *                                  | 0 | 0 | 2B | 2A | 1B | 1A | 0B | 0A |
     *                                  For exemple, 0b00001010 enable channel 0B and 1B
     *                                  all the other channels are desactivated.
    */
    inline void setOutputConfiguration(uint8_t Config) { POC=Config; }


    /*!
     * \brief pwm_setDutyCycle0     Set new duty-cycle for PWM 0
     *                              If PSC is locked, the changes will be taken into
     *                              account when the PSC will be unlocked
     * \param dutyCycle             Duty-cycle value
     *                              - Minimum value : 0(0%)
     *                              - Maximum value : PWM_COUNTER_MAX(100%)
     *				    dutyCcyle is automaticaly bounded if outside of range
     */
    void	setDutyCycle0(uint16_t dutyCycle);


    /*!
     * \brief pwm_setDutyCycle1     Set new duty-cycle for PWM 1
     *                              If PSC is locked, the changes will be taken into
     *                              account when the PSC will be unlocked
     * \param dutyCycle             Duty-cycle value
     *                              - Minimum value : 0(0%)
     *                              - Maximum value : PWM_COUNTER_MAX(100%)
     *				    dutyCcyle is automaticaly bounded if outside of range
     */
    void	setDutyCycle1(uint16_t dutyCycle);


    /*!
     * \brief pwm_setDutyCycle2     Set new duty-cycle for PWM 2
     *                              If PSC is locked, the changes will be taken into
     *                              account when the PSC will be unlocked
     * \param dutyCycle             Duty-cycle value
     *                              - Minimum value : 0(0%)
     *                              - Maximum value : PWM_COUNTER_MAX(100%)
     *				    dutyCcyle is automaticaly bounded if outside of range
     */
    void	setDutyCycle2(uint16_t dutyCycle);


    /*!
     * \brief pwm_unlock    Lock PWM update,
     *                      Used to synchronize updates
     *                      Changes will be taken into accound when unlocking
     */
    inline void unlock()    {PCNF &= ~(1<<PULOCK);}


    /*!
     * \brief pwm_lock      Unock PWM update, used to synchronize updates
     *                      All the changes are taken simultaneously
     */
    inline void lock()      {PCNF |=  (1<<PULOCK);}


    /*!
     * \brief pwm_setOutputConfiguration    Enable or disable each PWM channel
     * \param Config                        This variable is composed of 6 bits :
     *                                      | 0 | 0 | 2B | 2A | 1B | 1A | 0B | 0A |
     *                                      For exemple, 0b00001010 enable channel 0B and 1B
     *                                      all the other channels are desactivated.
     */
    inline void pwm_setOutputConfiguration(unsigned char Config) { POC=Config; }


private:

    // Internal PLL, can be used as source clock
    m32m1_pll		pll;

    // Current dead-time
    volatile uint8_t	deadTimeNbCycles;

    // Current counter max (PSC Output Compare Register)
    uint16_t		counterMax;


};

#endif // PWM_H
