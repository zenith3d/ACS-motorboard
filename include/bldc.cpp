#include "bldc.h"
#include "output.h"


m32m1_pwm       pwm;


// Current PWM duty cycle (speed of the motor)
volatile int        PWM_duty_cycle=0;

// Direction of rotation
volatile bool       RotationCW=false;



// Initialize BLDC motor
void bldc_init(unsigned char prescaler,
               unsigned char sourceClock,
               unsigned char deadTimeNumberCycles)
{
    // Configure ports as output and set low for MOSFET Drivers (disactivate all transistors)
    Output PSCOUT0A (&PSCOUT0A_PORT,PSCOUT0A_PIN); PSCOUT0A.setLow();
    Output PSCOUT0B (&PSCOUT0B_PORT,PSCOUT0B_PIN); PSCOUT0B.setLow();
    Output PSCOUT1A (&PSCOUT1A_PORT,PSCOUT1A_PIN); PSCOUT1A.setLow();
    Output PSCOUT1B (&PSCOUT1B_PORT,PSCOUT1B_PIN); PSCOUT1B.setLow();
    Output PSCOUT2A (&PSCOUT2A_PORT,PSCOUT2A_PIN); PSCOUT2A.setLow();
    Output PSCOUT2B (&PSCOUT2B_PORT,PSCOUT2B_PIN); PSCOUT2B.setLow();

    // Initialize PWM module (PSC Power Stage Controller)
    pwm.init(prescaler,sourceClock,deadTimeNumberCycles);

    // Initialize hall effect sensors
    hall_init();

    // Enable motor (configure ports and attach interrupt)
    bldc_enableMotor();

    // Enable interrupts
    sei();
}



// Enable or re-enable the motor
void bldc_enableMotor()
{
    // Lock PSC module
    pwm.lock();
    // Open the bottom transistors in the H-Bridge
    PORTB &= ~((1<<PB1) | (1<<PB6) | (1<<PB7));
    // Attach the commutation phase funtion to the hall effect sensors
    hall_attachInterrupt(onInterruptHallChange);
    //timer1_attachInterrupt(onInterruptTimer1);
    // Lock PSC module
    pwm.unlock();
}


// Disable the motor (phase are disconnected)
void bldc_disableMotor()
{
    // Lock PSC module
    pwm.lock();
    // Detach the commutation phase funtion to the hall effect sensors
    hall_detachInterrupt();
    //timer1_detachInterrupt();
    // Open the bottom transistors in the H-Bridge
    PORTB &= ~((1<<PB1) | (1<<PB6) | (1<<PB7));
    // Disable PWM from pins (outputs are standard ports)
    pwm.setOutputConfiguration(PWM_CONFIG_DISABLE_ALL);
    // Unlock PSC module
    pwm.unlock();
}



// Brakes the motor, each phase is connected to the ground
void bldc_brakeMotor()
{
    // Lock PSC module
    pwm.lock();
    // Detach the commutation phase funtion to the hall effect sensors
    hall_detachInterrupt();
    //timer1_detachInterrupt();
    // Disable PWM from pins (outputs are standard ports)
    pwm.setOutputConfiguration(PWM_CONFIG_DISABLE_ALL);
    // Close the bottom transistors in the H-Bridge
    // The three phases are connected to the ground
    PORTB |= (1<<PB1) | (1<<PB6) | (1<<PB7);
    // Unlock PSC module
    pwm.unlock();

}




// Set the speed of the motor
void bldc_setSpeed(int Speed)
{
    // Update the expected PWM duty cycle on each channel
    // The following instructions can not be interrupted
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Update PWM_Duty_cycle
        PWM_duty_cycle=abs(Speed);
        // Set rotation direction
        RotationCW=(Speed<0);        
        // Update duty cycle
        uint8_t hall=hall_getSensors();
        bldc_commutation(hall);

    }    
}


// Called on hall sensors change
void onInterruptHallChange(uint8_t Hall)
{    
    bldc_commutation(Hall);
}


// This function manage phase commutation
void bldc_commutation(uint8_t Hall)
{
    // Lock PWM to avoid transtory unexpected changes
    pwm.lock();

    // Commute according to the requested rotation direction,
    if (RotationCW)
    {
        // ClockWise

        switch (Hall)
        {
        case BLDC_PHASE_I:
            pwm.setOutputConfiguration(BLDC_SET_Q2L_Q1H);
            pwm.setDutyCycle0(PWM_duty_cycle);
            pwm.setDutyCycle1(0);
            break;
        case BLDC_PHASE_II:
            pwm.setOutputConfiguration(BLDC_SET_Q3L_Q1H);
            pwm.setDutyCycle0(PWM_duty_cycle);
            pwm.setDutyCycle2(0);
            break;
        case BLDC_PHASE_III:
            pwm.setOutputConfiguration(BLDC_SET_Q3L_Q2H);
            pwm.setDutyCycle1(PWM_duty_cycle);
            pwm.setDutyCycle2(0);
            break;
        case BLDC_PHASE_IV:
            pwm.setOutputConfiguration(BLDC_SET_Q1L_Q2H);
            pwm.setDutyCycle0(0);
            pwm.setDutyCycle1(PWM_duty_cycle);
            break;
        case BLDC_PHASE_V:
            pwm.setOutputConfiguration(BLDC_SET_Q1L_Q3H);
            pwm.setDutyCycle0(0);
            pwm.setDutyCycle2(PWM_duty_cycle);
            break;
        case BLDC_PHASE_VI:
            pwm.setOutputConfiguration(BLDC_SET_Q2L_Q3H);
            pwm.setDutyCycle1(0);
            pwm.setDutyCycle2(PWM_duty_cycle);
            break;
        }


    }
    else
    {

        // Counter ClockWise

        switch (Hall)
        {
        case BLDC_PHASE_IV:
            pwm.setOutputConfiguration(BLDC_SET_Q2L_Q1H);
            pwm.setDutyCycle0(PWM_duty_cycle);
            pwm.setDutyCycle1(0);
            break;
        case BLDC_PHASE_V:
            pwm.setOutputConfiguration(BLDC_SET_Q3L_Q1H);
            pwm.setDutyCycle0(PWM_duty_cycle);
            pwm.setDutyCycle2(0);
            break;
        case BLDC_PHASE_VI:
            pwm.setOutputConfiguration(BLDC_SET_Q3L_Q2H);
            pwm.setDutyCycle1(PWM_duty_cycle);
            pwm.setDutyCycle2(0);
            break;
        case BLDC_PHASE_I:
            pwm.setOutputConfiguration(BLDC_SET_Q1L_Q2H);
            pwm.setDutyCycle0(0);
            pwm.setDutyCycle1(PWM_duty_cycle);
            break;
        case BLDC_PHASE_II:
            pwm.setOutputConfiguration(BLDC_SET_Q1L_Q3H);
            pwm.setDutyCycle0(0);
            pwm.setDutyCycle2(PWM_duty_cycle);
            break;
        case BLDC_PHASE_III:
            pwm.setOutputConfiguration(BLDC_SET_Q2L_Q3H);
            pwm.setDutyCycle1(0);
            pwm.setDutyCycle2(PWM_duty_cycle);
            break;
        }
    }
    // Unlock PWM all the updated parameters are set simultaneously
    pwm.unlock();
}
