#include "m32m1_pll.h"

// Constructor, do nothing
m32m1_pll::m32m1_pll()
{}

/*
// Constructor, initialize the PLL to the requested frequency
// and wait until the PLL is ready
m32m1_pll::m32m1_pll(bool PLL_Factor)
{
    this->setFrequency(PLL_Factor);
    this->start();
    while (!this->isReady());
}
*/

// Start the PLL (needs about 100ms to start, check pll_isReady)
void m32m1_pll::start()
{
    // Enable PLL
    PLLCSR |= (1<<PLLE);
}



// Set PLL output frequency (64MHz or 32MHz)
void m32m1_pll::setFrequency(bool PLL_Factor)
{
    if (PLL_Factor)
        // Set the bit PLLF of PLLCSR
        PLLCSR |= (1<<PLLF);
    else
        // Clear the bit PLLF of PLLCSR
        PLLCSR &= ~(1<<PLLF);
}



// Stop the PLL
void m32m1_pll::stop()
{
    // Unset bit PLLE of PLLCSR
    PLLCSR &= ~(1<<PLLE);
}




// Return true when the PLL is locked
bool m32m1_pll::isReady()
{
    // Check bit PLOCK of PLLCSR
    return (PLLCSR & (1<<PLOCK) );
}
