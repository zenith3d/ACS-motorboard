#ifndef PLL_H
#define PLL_H

#include <avr/io.h>

#define     PLL_FREQUENCY_64MHZ             true
#define     PLL_FREQUENCY_32MHZ             false


/*!
 * \brief The m32m1_pll class   Manage the internal PLL of the atmega32m1
 * \note                        This class must be a singleton
 */
class m32m1_pll
{
public:



    /*!
     * \brief m32m1_pll         Constructor, do nothing
     */
    m32m1_pll();

    /*!
     * \brief m32m1_pll         Constructor, initialize the PLL according to requested frequency
     *                          start the PLL and wait until the PLL is ready
     * \param Frequency_64MHz   requested frequency possible values are :
     *                          - PLL_FREQUENCY_32MHZ or PLL_FREQUENCY_64MHZ (default)
     */
//    m32m1_pll(bool PLL_Factor=PLL_FREQUENCY_64MHZ);



    /*!
     * \brief pll_setFrequency  Set the PLL factor (32MHz or 64MHz)
     * \param Frequency_64MHz   true, the PLL output is 64MHz
     *                          false, the PLL output is 32MHz
     */
    void setFrequency(bool PLL_Factor=PLL_FREQUENCY_64MHZ);

    /*!
     * \brief start Start PLL
     *              The PLL needs about 100ms to start
     *              Check pll_isReady before use
     */
    void start();


    /*!
     * \brief stop  Stop the PLL (disable PLLE=PLL enable)
     */
    void stop();


    /*!
     * \brief pll_isReady   check if the PLL is locked
     * \return              true if the PLL is ready
     *                      false if the PLL is not yet locked
     */
    bool isReady();

};




#endif // PLL_H
