#include "m32m1_pwm.h"


// Constructor, do nothing
m32m1_pwm::m32m1_pwm()
{}


// Initialize the PWM device at a given frequency
void m32m1_pwm::init(uint8_t    prescaler,
                     uint8_t    sourceClock,
                     uint8_t    deadTimeNumberCycles,
                     uint16_t   counterMaximum)
{
    // Disable all the PWM channels the outpouts are standard ports
    // This to avoid cross conduction during initialization
    this->setOutputConfiguration(PWM_CONFIG_DISABLE_ALL);

    // Enable overlap protection
    PMIC0 &= ~(1<<POVEN0);
    PMIC1 &= ~(1<<POVEN1);
    PMIC2 &= ~(1<<POVEN2);

    // Set the requested prescaler
    this->setPrescaler(prescaler);
    // Set the requested sourceClock
    this->setSourceClock(sourceClock);

    // Store the number of dead cycles
    deadTimeNbCycles=deadTimeNumberCycles;
    // Set the counter maximum value
    this->setCounterMax(counterMaximum);

    // For ADC synchronisation
    POCR0RA=1;

    // Configure PSC (lock, center aligned mode, outputs active high
    PCNF = (1<<PULOCK) | (1<<PMODE) | (1<<POPB) | (1<<POPA);

    // reste the PSC Complete Cycle bit
    PCTL &= ~(1<<PCCYC);

    // Start the PSC
    PCTL |= (1<<PRUN);
}




// Set the requested prescaler in register PCTL
void m32m1_pwm::setPrescaler(uint8_t prescaler)
{
    PCTL = prescaler | (PCTL & 0b00111111);
}



// Set source clock for the PWM device
void m32m1_pwm::setSourceClock(uint8_t sourceClock)
{
    // According to the requested source clock (CPU of PLL)
    switch (sourceClock)
    {
    case PWM_SOURCE_CLK_CPU_CLK :
        pll.stop();
        PCTL &= ~(1<<PCLKSEL);
        break;

    case PWM_SOURCE_CLK_PLL_32MHZ :
        pll.setFrequency(PLL_FREQUENCY_32MHZ);
        pll.start();
        while (!pll.isReady());
        PCTL |= (1<<PCLKSEL);
        break;

    case PWM_SOURCE_CLK_PLL_64MHZ :
        pll.setFrequency(PLL_FREQUENCY_64MHZ);
        pll.start();
        while (!pll.isReady());
        PCTL |= (1<<PCLKSEL);
        break;
    }
}


// Set the maximum value for the counter (output compare match)
void m32m1_pwm::setCounterMax(uint16_t counterMaximum)
{
    // Store the requested value
    counterMax=counterMaximum;
    // PWM maximum counter value (output compare match)
    POCR_RB=counterMax+deadTimeNbCycles-1;
}


// Set new duty-cycle for PWM 0
void m32m1_pwm::setDutyCycle0(uint16_t dutyCycle)
{    
    if (dutyCycle>counterMax) dutyCycle=counterMax;
    POCR0SA=dutyCycle;
    POCR0SB=dutyCycle+deadTimeNbCycles;    
}

// Set new duty-cycle for PWM 1
void m32m1_pwm::setDutyCycle1(uint16_t dutyCycle)
{
    if (dutyCycle>counterMax) dutyCycle=counterMax;
    POCR1SA=dutyCycle;
    POCR1SB=dutyCycle+deadTimeNbCycles;
}

// Set new duty-cycle for PWM 2
void m32m1_pwm::setDutyCycle2(uint16_t dutyCycle)
{
    if (dutyCycle>counterMax) dutyCycle=counterMax;
    POCR2SA=dutyCycle;
    POCR2SB=dutyCycle+deadTimeNbCycles;
}





