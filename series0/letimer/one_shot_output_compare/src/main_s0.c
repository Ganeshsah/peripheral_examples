/**************************************************************************//**
 * @file
 * @brief This project demonstrates output compare in EM2 using the LETIMER.
 * After 1 second an output pin is set high (see README)
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_letimer.h"

#include "bsp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD6 (Expansion Header 16) as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initLetimer(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable clock to the LE modules interface
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Select LFXO for the LETIMER
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Reload COMP0 on underflow, set output, and run in one-shot mode
  letimerInit.comp0Top  = true;
  letimerInit.ufoa0     = letimerUFOAToggle;
  letimerInit.repMode   = letimerRepeatOneshot;
  letimerInit.enable = false;
  letimerInit.topValue = 32000;

  // Need REP0 != 0 to run until the first event
  LETIMER_RepeatSet(LETIMER0, 0, 1);

  // Enable LETIMER0 output0 on PD6 (Route 0)
  LETIMER0->ROUTE |=  LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC0;

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit );

  // Compare on wake-up interval count
  LETIMER_Enable(LETIMER0,true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGpio();
  initLetimer();

  // Enter low energy state, PD6 will go high after 1 second
  while (1)
  {
    EMU_EnterEM2(true);
  }
}
