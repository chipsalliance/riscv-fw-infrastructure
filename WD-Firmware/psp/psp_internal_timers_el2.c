/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020-2021 Western Digital Corporation or its affiliates.
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
*/
/**
* @file   psp_internal_timers_el2.c
* @author Nati Rapaport
* @date   19.08.2020
* @brief  This file implements EL2 timers service functions
*
*/
/**
* include files
*/
#include "psp_api.h"



 
/**
* definitions
*/

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

/**
 * Internal functions
 */

/**
* APIs
*/

/**
* @brief setup Cascade timer (64 bit internal timer)
*
* Note :CASCADE 64'bit timer supports period in the range: FROM 0x100000000 TO  0xFFFFFFFEFFFFFFFF (only).
*
* @param - ullPeriodCycles - defines the timer's period in cycles
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerSetup(u64_t ullPeriodCycles)
{
  u32_t uiHi32, uiLo32, uicounter0 = 0;

  /* Disable incrementing timer counter: */
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);

  /* Least significant 32bit */
  uiLo32 = (u32_t)(ullPeriodCycles & D_PSP_LEAST_SIGNIFICANT_32_BIT_MASK);

  /* Most significant 32bit */
  uiHi32 = (u32_t)(ullPeriodCycles >> D_PSP_SHIFT_32);

  /* Check if the period on the range of  0x100000000 TO 0xFFFFFFFEFFFFFFFF */
  M_PSP_ASSERT(uiHi32>0 && uiHi32<D_PSP_32BIT_MASK);

  /* if the least significant 32'bit is not equal to zero edit timer0 counter (MITCNT0) and Timer1 bound (MITBND1) values  */
  if(uiLo32 > 0)
  {
    /* calculate timer0 counter(MITCNT0) value it should be ((FFFFFFFF-least significant 32'bit)+1) */
    uicounter0 = (D_PSP_32BIT_MASK-uiLo32) + 1;
    /* calculate Timer1 bound (MITBND1) it should be (Most significant 32bit +1)   */
    uiHi32 = uiHi32 + 1;
  }

  /* Set Timer1 bound (MITBND1) value */
  M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiHi32);

  /* Reset timer1 counter (MITCNT1) to Zero */
  M_PSP_WRITE_CSR(D_PSP_MITCNT1_NUM, 0);

  /* Set timer0 bound (MITBND0) to FFFFFFFF */
  M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, D_PSP_32BIT_MASK);

  /* Set timer0 counter (MITCNT0) */
  M_PSP_WRITE_CSR(D_PSP_MITCNT0_NUM, uicounter0);

  /* In cascade mode, the HALT-EN, and PAUSE-EN bits must be the set identically for both timers - so disable all of them them now */
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_HALT_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_HALT_EN_MASK);

  /* ENABLE CASCADE */
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_CASCADE);

 }

/**
* @brief Setup Internal core's Timer
*
* @param - uiTimer           - indicates which timer to setup
*
* @param - uiPeriodCycles  - defines the timer's period in cycles
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerCounterSetup(u32_t uiTimer, u32_t uiPeriodCycles)
{
  u32_t uiZero=0;
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      /* Disable Timer0 Counting */
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      /* Set Timer0 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiPeriodCycles);
      /* Reset Timer0 Counter */
      M_PSP_WRITE_CSR(D_PSP_MITCNT0_NUM, uiZero);
      /* DISABLE CASCADE */
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_CASCADE);
      break;
    case D_PSP_INTERNAL_TIMER1:
      /* Disable Timer1 Counting */
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      /* Set Timer1 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiPeriodCycles);
      /* Reset Timer1 Counter */
      M_PSP_WRITE_CSR(D_PSP_MITCNT1_NUM, uiZero);
      /* DISABLE CASCADE */
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_CASCADE);
      break;
    default:
      break;
  }
}

/**
* @brief Get Core 64'bit Internal Timer counter value
*/
D_PSP_TEXT_SECTION u64_t pspMachineInternalTimer64BitTimerCounterGet(void)
{
  u64_t ullCounter = 0;
  u64_t ullCounterTemp = 0;
  /* Most significant 32bit */
  ullCounterTemp = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
  ullCounter = ullCounterTemp << D_PSP_SHIFT_32;

  /* Most significant 32bit is Timer1 counter value(MITCNT1)
   * and Least significant 32bit is Timer0 counter value(MITCNT0) */
  ullCounter |= M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);

  return ullCounter;
}

/**
* @brief Get Core Internal Timer counter value
*
* @param - uitimer - indicates from which timer to get the counter value
*
* @return u32_t      - Timer counter value
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
D_PSP_TEXT_SECTION u32_t pspMachineInternalTimerCounterGet(u32_t uiTimer)
{
  u32_t uiCounter = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      uiCounter = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      uiCounter = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      break;
    default:
      break;
  }

  return uiCounter;
}

/**
* @brief Get Core 64'bit Internal Timer compare counter value
*/
D_PSP_TEXT_SECTION u64_t pspMachineInternalTimer64BitTimerCompareCounterGet(void)
{
  u64_t ullCounterCompare = 0;
  u64_t ullCounterCompareTemp = 0;

  /* Most significant 32bit */
  ullCounterCompareTemp = M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
  ullCounterCompare = ullCounterCompareTemp << D_PSP_SHIFT_32;

  /* Most significant 32bit is Timer1 bound value(MITBND1)
   * and Least significant 32bit is Timer0 bound value(MITBND0) */
  ullCounterCompare |= M_PSP_READ_CSR(D_PSP_MITBND0_NUM);

  return ullCounterCompare;
}

/**
* @brief Get Core Internal Timer compare counter value
*
* @param - uitimer - indicates from which timer to get the compare-counter value
*
* @return u32_t      – Time compare counter value
*
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
D_PSP_TEXT_SECTION u32_t pspMachineInternalTimerCompareCounterGet(u32_t uiTimer)
{
  u32_t uiCounterCompare = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      uiCounterCompare = M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      uiCounterCompare = M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      break;
    default:
      break;
  }

  return uiCounterCompare;
}

/**
* @brief Enable Core Internal 64'bit timer counting when core in sleep mode
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerEnableCountInSleepMode(void)
{
  M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_HALT_EN_MASK);
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_HALT_EN_MASK);
}

/**
* @brief Enable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to Enable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerEnableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable Core Internal 64'bit timer counting when core in sleep mode
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerDisableCountInSleepMode(void)
{
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_HALT_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_HALT_EN_MASK);
}

/**
* @brief Disable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to Disable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1s
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerDisableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Enable Core Internal 64'bit timer counting when core in Stall mode
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerEnableCountInStallMode(void)
{
  M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
}

/**
* @brief Enable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to Enable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerEnableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable Core Internal 64'bit timer counting when core in Stall mode
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerDisableCountInStallMode(void)
{
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
}

/**
* @brief Disable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to Disable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerDisableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }

}

/**
* @brief Enable incrementing internal 64'bit timer counter (Run)
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerRun(void)
{
  M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
}
/**
* @brief Enable incrementing timer counter (Run)
*
* @param - uiTimer  - indicates which timer to Run
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerRun(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable incrementing internal 64'bit timer counter (Pause)
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimer64BitTimerPause(void)
{
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
}

/**
* @brief Disable incrementing timer counter
*
* @param - uiTimer  - indicates which timer to disable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerPause(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    default:
      break;
  }
}

