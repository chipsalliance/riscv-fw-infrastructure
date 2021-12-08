/*
* SPDX-License-Identifier: Apache-2.0
*  Copyright 2021 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

/**
* @file   demo_internal_timers.c
* @authors Nati Rapaport, Ibrahim Qashqoush
* @date   28.11.2021
* @brief  Demo application for SWERV internal timers
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"


/**
 * definitions
 */

#define D_DEMO_INTERRUPTS_ENABLE_IN_HALT                  1
#define D_DEMO_MTIMER_64BIT_SLEEP_C                       0x100FFFFFF /* Period in cycles */
#define D_DEMO_CASCADE_64BIT_SLEEP_C                      0x10000000F /* Period in cycles */
#define D_DEMO_TIMER0_NUM_OF_ISRS                         30
#define D_DEMO_TIMER1_NUM_OF_ISRS                         55
#define D_DEMO_TIMER0_PERIOD_MS                           50
#define D_DEMO_TIMER1_PERIOD_MS                           175
#define D_DEMO_TIMER0_LONG_PERIOD_CYCLES                  0x0FEAFCF1
#define D_DEMO_TIMER1_LONG_PERIOD_CYCLES                  0x0FFAFD01
#define D_DEMO_TIMER1_NUM_OF_TICKS_FOR_64BIT_TEST         0x100000FFF
#define D_DEMO_CASCADE_TIMER_COUNTER_GET_PERIOD_CYCLES    0x18800000006 /*Don't change it */
#define D_DEMO_CASCADE_TIMER_MITCNT1                      0x00000080    /*Don't change it */
#define D_DEMO_CASCADE_TIMER_MITCNT0                      0xFFFFFFFA    /*Don't change it */
#define D_DEMO_TEMP_BOUND                                 0x0000FFFF
#define D_DEMO_ZERO                                       0

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/
volatile u32_t g_uiMachineTimerIsrCount;
volatile u32_t g_uiTimer0IsrCount;
volatile u32_t g_uiTimer1IsrCount;
volatile u32_t g_uiCascadeTimerIsrCount;
volatile u32_t g_sleepDemoFlag;


/**
* functions
*/


/**-------------------------------------
 *    ISR's (Interrupt Service Routine)
 * -------------------------------------
 */

/**
 * @brief - This function counts the number of internal timer0 interrupts.
 * If the counter is equal or greater than D_DEMO_TIMER0_NUM_OF_ISRS it Disables internal timer0 interrupts.
 */
void demoInternalTimer0Isr(void)
{
  /* Disable internal timer0 interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0);

  /* g_uiTimer0IsrCount counting timer0 interrupts */
  g_uiTimer0IsrCount++;

  /* If Number of internal timer0 interrupts less than D_DEMO_TIMER0_NUM_OF_ISRS enable timer0 interrupt */
  if (D_DEMO_TIMER0_NUM_OF_ISRS > g_uiTimer0IsrCount)
  {
    pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0);
  }

}

/**
 * @brief - This function counts the number of internal timer1 interrupts.
 * If the counter is equal or greater than D_DEMO_TIMER1_NUM_OF_ISRS it Disables internal timer1 interrupts.
 *
 */
void demoInternalTimer1Isr(void)
{
  /* Disable internal timer1 interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* g_uiTimer1IsrCount counts internal timer1 interrupts */
  g_uiTimer1IsrCount++;

  /* If Number of internal timer1 interrupts less than D_DEMO_TIMER1_NUM_OF_ISRS enable timer1 interrupt */
  if (D_DEMO_TIMER1_NUM_OF_ISRS > g_uiTimer1IsrCount)
  {
    pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);
  }
}


/**
 * @brief -  This function check if internal timer0 interrupt occur before internal timer1 interrupt.
 *
 * Relevant for Sleep and stall mode Demos (executed when timer1 interrupt occurs)
 */
void demoInternalTimer1SleepOrStallModeIsr(void)
{
  /* Disable timer1 interrupts */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* If timer0 interrupt occur print error and enters an endless loop */
  if(g_uiTimer0IsrCount > 0)
  {
    M_DEMO_ENDLESS_LOOP();
  }

  /* Rise the flag */
  g_sleepDemoFlag++;
}

/**
 * @brief - In case that Cascade Timer interrupt occurs, raise the flag and disable Cascade Timer interrupts
 */
void demoInternalTimerCascadeTimerIsr(void)
{

  /* Disable Cascade timer interrupts */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* g_uiCascadeTimerIsrCount counts the number of cascade timer interrupts */
  g_uiCascadeTimerIsrCount++;

}

/**
 * @brief - This function check if internal Cascade timer interrupt occur before machine timer interrupt.
 *  (executed when machine timer interrupt occurs)
 */
void demoInternalTimerCascadeTimerDisableCountInSleepModeIsr(void)
{
  /* Disable machine timer interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);
  /* IF cascade timer interrupt occur print error and enters an endless loop */
  if(g_uiCascadeTimerIsrCount > 0)
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();
  }
  g_sleepDemoFlag++;
}




/**
 * @brief - Demo for timer0 and timer1 interrupt setup and ISRs
 */
void demoInternalTimerInterrupts(void)
{
  u32_t uiInterruptsStatus, uiCycles;


  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Register Timer0 and Timer1 ISRs */
  pspMachineInterruptsRegisterIsr(demoInternalTimer0Isr, E_MACHINE_INTERNAL_TIMER0_CAUSE);
  pspMachineInterruptsRegisterIsr(demoInternalTimer1Isr, E_MACHINE_INTERNAL_TIMER1_CAUSE);

  /* Setup Timer0 and Timer1 */
  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER0_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER0, uiCycles);

  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER1_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER1, uiCycles);

  /* Enable Timer0/Timer1 interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0);
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* Enable interrupts */
  pspMachineInterruptsEnable();

  /* Run timers */
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER0);
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER1);

  /* Loop until all timer ISRs will complete */
  while ((D_DEMO_TIMER0_NUM_OF_ISRS > g_uiTimer0IsrCount) || (D_DEMO_TIMER1_NUM_OF_ISRS > g_uiTimer1IsrCount))
  {
    M_PSP_NOP();
  }

}

/**
 * @brief -This function demonstrate the activation of Timer0/Timer1 in sleep mode.
 * Enabling timer1 counting in sleep mode and disabling timer0 counting in sleep mode.
 */
void demoTimerSleepMode(void)
{
  u32_t uiInterruptsStatus, uiCycles;


  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Reset variables */
  g_uiTimer0IsrCount = 0;
  g_sleepDemoFlag = 0;

  /* Register Timer0 and Timer1 ISRs */
  pspMachineInterruptsRegisterIsr(demoInternalTimer0Isr, E_MACHINE_INTERNAL_TIMER0_CAUSE);
  pspMachineInterruptsRegisterIsr(demoInternalTimer1SleepOrStallModeIsr, E_MACHINE_INTERNAL_TIMER1_CAUSE);

  /* Enable timers interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0);
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* Disable timer0 in sleep mode */
  pspMachineInternalTimerDisableCountInSleepMode(D_PSP_INTERNAL_TIMER0);
  /* Enable timer1 in sleep mode */
  pspMachineInternalTimerEnableCountInSleepMode(D_PSP_INTERNAL_TIMER1);

  /* Setup Timer0 */
  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER0_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER0, uiCycles);

  /* Setup Timer1
   * Timer1 period is greater then Timer0 – to verify that Timer0 did not tick in sleep mode. */
  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER1_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER1, uiCycles);

  /* Run timers */
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER1);
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER0);

  /* Sleep and enable interrupts */
  pspMachinePowerMngCtrlHalt(D_DEMO_INTERRUPTS_ENABLE_IN_HALT);

  /* Wait for interrupt */
  while(g_sleepDemoFlag == 0)
  {
    M_PSP_NOP();
  }

}


/**
 * @brief - This function demonstrate Getting internal Timer0/Timer1 counter and bound values.
 */
void demoInternalTimerCounterAndBound(void)
{

/**************Part1 timer Bound *************/

  u64_t ullTimerCountAtTheBeginning, ullTimerCompare, ullTimerCountAtTheEnd;
  u32_t uiTimers[] = {D_PSP_INTERNAL_TIMER0, D_PSP_INTERNAL_TIMER1};
  u32_t uiIndex, uiTimersSize = 2;

  /* Setup Timer0 */
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER0, D_DEMO_TIMER0_LONG_PERIOD_CYCLES);

  /* Get timer0 bound */
  ullTimerCompare = pspMachineInternalTimerCompareCounterGet(D_PSP_INTERNAL_TIMER0);
  /* Compare the value from Get bound API to defined bound */
  if(ullTimerCompare != D_DEMO_TIMER0_LONG_PERIOD_CYCLES)
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();
  }

  /* Setup Timer1*/
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER1, D_DEMO_TIMER1_LONG_PERIOD_CYCLES);

  /* Get timer1 bound */
  ullTimerCompare = pspMachineInternalTimerCompareCounterGet(D_PSP_INTERNAL_TIMER1);
  /* Check timer1 bound */
  if(ullTimerCompare != D_DEMO_TIMER1_LONG_PERIOD_CYCLES)
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();
  }


/*********************Part2 Internal timer counter **************/

  /* Run the timers */
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER0);
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER1);

  /* For each timer */
  for(uiIndex = 0; uiIndex < uiTimersSize; uiIndex++)
  {
    /* Set timer counter to zero */
    (uiIndex == 0) ? M_PSP_WRITE_CSR(D_PSP_MITCNT0_NUM, 0) : M_PSP_WRITE_CSR(D_PSP_MITCNT1_NUM, 0);

    /* Wait */
    M_PSP_NOP();

    /* Get timer counter (first call) */
    ullTimerCountAtTheBeginning = pspMachineInternalTimerCounterGet(uiTimers[uiIndex]);

    /* Wait */
    M_PSP_NOP();
    M_PSP_NOP();

    /* Get timer counter (second call) */
    ullTimerCountAtTheEnd = pspMachineInternalTimerCounterGet(uiTimers[uiIndex]);

    /* If the number that we get in the second call is less than the first call we have an error */
    if(ullTimerCountAtTheEnd <= ullTimerCountAtTheBeginning)
    {
      /* Print error and enters an endless loop */
      M_DEMO_ENDLESS_LOOP();
    }
  }



}



/**
* @brief - This function demonstrate the activation of timer0/timer1 in Stall mode
*/
void demoTimersStallMode(void)
{

  u32_t uiInterruptsStatus, uiCycles;

  /* Reset variables */
  g_uiTimer0IsrCount = 0;
  g_sleepDemoFlag = 0;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Register  Timer0 and Timer1 ISRs */
  pspMachineInterruptsRegisterIsr(demoInternalTimer0Isr, E_MACHINE_INTERNAL_TIMER0_CAUSE);
  pspMachineInterruptsRegisterIsr(demoInternalTimer1SleepOrStallModeIsr, E_MACHINE_INTERNAL_TIMER1_CAUSE);

  /* Enable Timer0/Timer1 interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0);
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);

  /* Disable timer0 counting in stall mode */
  pspMachineInternalTimerDisableCountInStallMode(D_PSP_INTERNAL_TIMER0);
  /* Enable timer1 counting in stall mode */
  pspMachineInternalTimerEnableCountInStallMode(D_PSP_INTERNAL_TIMER1);

  /* Setup Timer0 and Timer1
   * Timer1 period is greater then the Timer0 – to verify that the timer0 did not tick in stall mode.  */
  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER0_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER0, uiCycles);

  uiCycles = M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER1_PERIOD_MS);
  pspMachineInternalTimerCounterSetup(D_PSP_INTERNAL_TIMER1, uiCycles);


  /* Enable interrupt */
  pspMachineInterruptsEnable();

  /* Run Timers */
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER1);
  pspMachineInternalTimerRun(D_PSP_INTERNAL_TIMER0);

  /* Pause(stall) for D_DEMO_TIMER0_LONG_PERIOD_CYCLES cycles (long period) */
  pspMachinePowerMngCtrlStall(D_DEMO_TIMER0_LONG_PERIOD_CYCLES);

  /* Wait to timer1 interrupt */
  while(g_sleepDemoFlag == 0)
  {
    M_PSP_NOP();
  }
}

#if defined(D_SWERV_EH2) ||defined(D_SWERV_EL2)


/**
 * @brief – This function demonstrate the activation of 64bit Timer in sleep mode
 *
 * Part1: Disable timer ticking in sleep mode
 *
 * Part2: Enable timer ticking in sleep mode
 */
void demo64bitTimerSleep(void)
{
/* PART1  pspMachineInternalTimerDisableCountInSleepMode API */

  u32_t uiInterruptsStatus;

  /* Reset variables */
  g_uiCascadeTimerIsrCount = 0;
  g_sleepDemoFlag = 0;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Register machine timer and cascade timer(timer1) ISRs */
  pspMachineInterruptsRegisterIsr(demoInternalTimerCascadeTimerIsr, E_MACHINE_INTERNAL_TIMER1_CAUSE);
  pspMachineInterruptsRegisterIsr(demoInternalTimerCascadeTimerDisableCountInSleepModeIsr, D_PSP_INTERRUPTS_MACHINE_TIMER);

  /* Enable timers interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);/* machine timer */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);/* internal timer1 */

  /* Setup Cascade timer */
  pspMachineInternalTimer64BitTimerSetup(D_DEMO_CASCADE_64BIT_SLEEP_C);/* as default we disable count in sleep mode */

  /* Disable Cascade timer counting in sleep mode */
  pspMachineInternalTimer64BitTimerDisableCountInSleepMode();

  /* Setup and run machine timer.
   * Machine timer’s period is greater then the 64bit timer – to verify that the 64bit timer did not tick in sleep mode. */
  pspMachineTimerCounterSetupAndRun(D_DEMO_MTIMER_64BIT_SLEEP_C);

  /* Run Cascade timer */
  pspMachineInternalTimer64BitTimerRun();

  /* Sleep and enable interrupts */
  pspMachinePowerMngCtrlHalt(D_DEMO_INTERRUPTS_ENABLE_IN_HALT);

  /* Wait until machine timer interrupt occur(machine timer period greater than cascade timer period) */
  while(g_sleepDemoFlag == 0)
  {
    M_PSP_NOP();
  }



/* PART2  pspMachineInternalTimerEnableCountInSleepMode API */

  /* Reset variable */
  g_uiCascadeTimerIsrCount = 0;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Disable incrementing timer counter */
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);

  /* Before enabling timer interrupt we set different value in bound and counter to avoid the case:bound==counter */
  M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, D_DEMO_TEMP_BOUND);
  M_PSP_WRITE_CSR(D_PSP_MITCNT1_NUM, D_DEMO_ZERO);

  /* Register cascade timer(timer1) ISRs */
  pspMachineInterruptsRegisterIsr(demoInternalTimerCascadeTimerIsr, E_MACHINE_INTERNAL_TIMER1_CAUSE);

  /* Enable Cascade timer interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1);//internal timer1


  /* Setup Cascade Timer */
  pspMachineInternalTimer64BitTimerSetup(D_DEMO_CASCADE_64BIT_SLEEP_C);

  /* Enable incrementing cascade timer counter in sleep mode */
  pspMachineInternalTimer64BitTimerEnableCountInSleepMode();

  /* Run cascade timer */
  pspMachineInternalTimer64BitTimerRun();

  /* Sleep and enable interrupts */
  pspMachinePowerMngCtrlHalt(D_DEMO_INTERRUPTS_ENABLE_IN_HALT);

  /* Wait to machine timer interrupt */
  while(g_uiCascadeTimerIsrCount == 0)
  {
    M_PSP_NOP();
  }
}


/**
 * @brief - This function demonstrate Getting internal 64'bit Timer counter and bound values.
 */
void  demo64bitTimerCounterAndBound()
{

  u64_t  ullTemp, ullTimerCountAtTheBeginning, ullTimerCountAtTheEnd;
  u32_t uiHIComparetime, uiLoComparetime, uiHIDemoTimerPERIOD, uiLoDemoTimerPERIOD;


/* PART 1 : pspMachineInternalTimerCompareCounterGet */


  /* Setup Cascade timer */
  pspMachineInternalTimer64BitTimerSetup(D_DEMO_TIMER1_NUM_OF_TICKS_FOR_64BIT_TEST);

  /* Get Cascade timer Compare(bound) */
  ullTemp = pspMachineInternalTimer64BitTimerCompareCounterGet();

  /* Least significant  32'bit (CompareCounterGet API value) */
  uiLoComparetime = (u32_t)ullTemp;

  /* Most significant 32'bit (CompareCounterGet API value) */
  uiHIComparetime = (u32_t)(ullTemp >> D_PSP_SHIFT_32);

  /* Period Least significant 32'bit */
  uiLoDemoTimerPERIOD = (u32_t)D_DEMO_TIMER1_NUM_OF_TICKS_FOR_64BIT_TEST;
  /* Check if Period Least significant 32'bit not equal to zero */
  if(uiLoDemoTimerPERIOD != 0)
    uiHIComparetime = uiHIComparetime - 1;/* Because in timer setup API we add 1 to most significant 32'bits */

  /* Period Most significant 32'bit */
  uiHIDemoTimerPERIOD = (u32_t)(D_DEMO_TIMER1_NUM_OF_TICKS_FOR_64BIT_TEST >> D_PSP_SHIFT_32);

  /* If Period most significant 32'bit and ((GET value Most significant 32'bit)-1) not the same means we have an error */
  if((uiHIComparetime != uiHIDemoTimerPERIOD)||(uiLoComparetime != D_PSP_32BIT_MASK))
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();
  }

/* PART 2 : pspMachineInternalTimerCounterGet */

  /* Disable Cascade timer Counting */
  pspMachineInternalTimer64BitTimerPause();

  /**
   * Setup Cascade Timer(counter and bound)
   * as a result of the setup:
   * MITBND0= FFFFFFFF
   * MITCNT0= 0xFFFFFFFA
   * MITBND1= 0x188
   */
  pspMachineInternalTimer64BitTimerSetup(D_DEMO_CASCADE_TIMER_COUNTER_GET_PERIOD_CYCLES);

  /* set MITCNT1 to 0x80 (Start to count from 0x80FFFFFFFA) */
  M_PSP_WRITE_CSR( D_PSP_MITCNT1_NUM, D_DEMO_CASCADE_TIMER_MITCNT1);

  /* Get timer Counter (it should have the same value as the setup because we Disabled counting) */
  ullTemp = pspMachineInternalTimer64BitTimerCounterGet();

  /* Least significant 32'bit */
  uiLoComparetime = (u32_t)ullTemp;

  /* Most significant 32'bit */
  uiHIComparetime = (u32_t)(ullTemp >> D_PSP_SHIFT_32);

  /* If the get value not the same as MITCNT1|MITCNT0 i.e 0x80FFFFFFFA then we have an error */
  if((uiLoComparetime != D_DEMO_CASCADE_TIMER_MITCNT0)||(uiHIComparetime != D_DEMO_CASCADE_TIMER_MITCNT1 ))
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();
  }

/* PART 2.2 */

  /* Run the timer */
  pspMachineInternalTimer64BitTimerRun();
  M_PSP_NOP();

  /* Get timer counter (first call) */
  ullTimerCountAtTheBeginning = pspMachineInternalTimer64BitTimerCounterGet();

  /* Wait */
  M_PSP_NOP();
  M_PSP_NOP();

  /* Get timer counter (second call) */
  ullTimerCountAtTheEnd = pspMachineInternalTimer64BitTimerCounterGet();

  /* If the number that we get in the second call is less than the first call we have an error */
  if(ullTimerCountAtTheEnd <= ullTimerCountAtTheBeginning)
  {
    /* Print error and enters an endless loop */
    M_DEMO_ENDLESS_LOOP();

  }

}

#endif /* D_SWERV_EH2 || D_SWERV_EL2 */


/**
 * @brief - demoStart - startup point of the demo application. called from main function.
 */
void demoStart(void)
{
  M_DEMO_START_PRINT();
  /* Register interrupt vector */
  pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

  /* Run this demo only if target is Swerv. Cannot run on Whisper */
  if (D_PSP_TRUE == demoIsSwervBoard())
  {
#if defined(D_SWERV_EH2) || defined(D_SWERV_EL2)

    /* Demo #1 - Internal 64'bit timer sleep mode - relevant for SweRV EL2/EH2 only */
    demo64bitTimerSleep();

    /* Demo #2 - Getting internal 64'bit Timer counter and bound values - relevant for SweRV EL2/EH2 only */
    demo64bitTimerCounterAndBound();

#endif

  /* Demo #3 - Simple demo for timer0 and timer1 ISRs */
  demoInternalTimerInterrupts();

  /* Demo #4 - Internal timer0 and timer1 sleep mode */
  demoTimerSleepMode();

  /* Demo #5 - Getting internal Timer0/Timer1 counter and bound values */
  demoInternalTimerCounterAndBound();

  /* Demo #6 - timer0 and timer1  Stall Mode */
  demoTimersStallMode();

  }
  else
  /* whisper */
  {
    printfNexys("This demo can't run under whisper");
  }

  M_DEMO_END_PRINT();
}
