#include "hall.h"





// ________________________
// ::: Global variables :::



// Direction of motion (0=CW 1=CCW)
volatile bool hall_Direction=0;

// Hall sensors previous status
volatile uint8_t hall_previousSensors=0;

// Hall sensor error
volatile bool hall_ErrorHallSensors;

// Current motor position
volatile int64_t hall_Position;


void (*userFunction)(unsigned char)=0;




// _______________________
// ::: Initializations :::


// Initialize hall sensors and interruptions
void hall_init()
{
    // Set hall sensor ports as input
    HALL_1_DDR &= ~(1<<HALL_1_BIT);
    HALL_2_DDR &= ~(1<<HALL_2_BIT);
    HALL_3_DDR &= ~(1<<HALL_3_BIT);

    // Enable pin change interrupts (1 and 2)
    PCICR |= (1<<PCIE1) | (1<<PCIE2);

    // Enable pin change interrupt for each hall sensor
    HALL_1_PCMSK |=1<<HALL_1_PCINT;
    HALL_2_PCMSK |=1<<HALL_2_PCINT;
    HALL_3_PCMSK |=1<<HALL_3_PCINT;

    // Default initial direction is CW
    hall_Direction=0;
    // No error after initialization
    hall_ErrorHallSensors=0;
    // Read sensors to initialize previous sensor value
    hall_previousSensors=hall_getSensors();
    // Reset motor position
    hall_Position=0;
}








// _________________
// ::: Interrupt :::

// Attach a funtion to the hall sensor change interrupt
void hall_attachInterrupt( void (*ptrFonction)(unsigned char))
{
    userFunction=ptrFonction;
}



// Detach the function from the hall sensor change interrupt
void hall_detachInterrupt()
{
    userFunction=0;
}



// Interrupt vectors, called everytime a change is detected on H2
ISR(PCINT1_vect)
{
    // Get current value of hall sensors
    uint8_t currentStatus=hall_getSensors();

    // If an interrupt is attached, call the user function
    if (userFunction)   (*userFunction)(currentStatus);

    // Update position and direction
    hall_onChange(currentStatus);
}





// Interrupt vectors, called everytime a change is detected on H1 or H3
ISR(PCINT2_vect)
{
    // Get current value of hall sensors
    uint8_t currentStatus=hall_getSensors();

    // If an interrupt is attached, call the user function
    if (userFunction)   (*userFunction)(currentStatus);

    // Update position and direction
    hall_onChange(currentStatus);

}






// ___________________________
// ::: Getters and setters :::



// Return the current sensor ( 0 | 0 | 0 | 0 | 0 | H3| H2 | H1 )
uint8_t hall_getSensors()
{
    return  (HALL_1_PIN&(1<<HALL_1_BIT))>>HALL_1_BIT |
            ((HALL_2_PIN&(1<<HALL_2_BIT))>>HALL_2_BIT)<<1 |
            ((HALL_3_PIN&(1<<HALL_3_BIT))>>HALL_3_BIT)<<2;
}


// Return the direction of rotation (0=CW 1=CCW)
bool hall_getDirection()
{
    return hall_Direction;
}







// Return the current position of the motor (number of steps)
int64_t hall_getPosition()
{
    int64_t PositionCopy;
    // The following instruction can not be interrupted
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Copy the position shared with the interruption service routine
        PositionCopy=hall_Position;
    }
    return PositionCopy;
}


// Set the current position
void hall_setPosition(int64_t Position)
{
    hall_Position=Position;
}


// Return true if an error occured during processing hall sensors
bool hall_getError()
{
    return hall_ErrorHallSensors;
}


// Reset hall sensor error
void hall_resetError()
{
    hall_ErrorHallSensors=false;
}



// Manage hall sensors (call on every sensor change)
void hall_onChange(uint8_t currentStatus)
{
    // No change or insignificant transition
    if (currentStatus==hall_previousSensors) return;

    // Create a byte containing previous value (bits 3,4 and 5) and current value (bits 0,1 and 2)
    unsigned char Status = currentStatus | (hall_previousSensors<<3);

    // Analyse the status (combination previous + current)
    switch (Status)
    {
        // CW
        case 0b101001: hall_Direction=HALL_DIRECTION_CW; break;
        case 0b001011: hall_Direction=HALL_DIRECTION_CW; break;
        case 0b011010: hall_Direction=HALL_DIRECTION_CW; break;
        case 0b010110: hall_Direction=HALL_DIRECTION_CW; break;
        case 0b110100: hall_Direction=HALL_DIRECTION_CW; break;
        case 0b100101: hall_Direction=HALL_DIRECTION_CW; break;

        // CCW
        case 0b011001: hall_Direction=HALL_DIRECTION_CCW; break;
        case 0b010011: hall_Direction=HALL_DIRECTION_CCW; break;
        case 0b110010: hall_Direction=HALL_DIRECTION_CCW; break;
        case 0b100110: hall_Direction=HALL_DIRECTION_CCW; break;
        case 0b101100: hall_Direction=HALL_DIRECTION_CCW; break;
        case 0b001101: hall_Direction=HALL_DIRECTION_CCW; break;

        // Error, a problem occured, report an error
        default : hall_ErrorHallSensors=true;
    }

    // Increase or decrease the position according to the direction of motion
    if (hall_Direction==HALL_DIRECTION_CCW) hall_Position++; 
    else hall_Position--;

    // Current value becomes previous value for the next call of the function
    hall_previousSensors=currentStatus;
}



