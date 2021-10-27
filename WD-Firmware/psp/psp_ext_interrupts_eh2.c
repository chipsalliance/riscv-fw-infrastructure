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
* @file   psp_ext_interrupts_eh2.c
* @author Nati Rapaport
* @date   18.05.2020
* @brief  The file supplies external interrupt services (vector table installment, ISRs registration, PIC control API etc)
*         The file is specific to SweRV EH2 specifications.
*
*/

/**
* include files
*/
#include "psp_api.h"
#include "psp_internal_mutex_eh2.h"

/**
* definitions
*/

/* Number of external interrupt sources in the PIC */
#if (0 == D_PIC_NUM_OF_EXT_INTERRUPTS)
    #error "D_PIC_NUM_OF_EXT_INTERRUPTS is not defined"
#else
    #define D_PSP_PIC_NUM_OF_EXT_INTERRUPTS D_PIC_NUM_OF_EXT_INTERRUPTS
#endif

/* Number of first External interrupt source in the PIC (0 is a valid number) */
#ifndef  D_EXT_INTERRUPT_FIRST_SOURCE_USED
    #error "D_EXT_INTERRUPT_FIRST_SOURCE_USED is not defined"
#else
    #define D_PSP_EXT_INTERRUPT_FIRST_SOURCE_USED    D_EXT_INTERRUPT_FIRST_SOURCE_USED
#endif

/* Number of last External interrupt source in the PIC */
#if (0 == D_EXT_INTERRUPT_LAST_SOURCE_USED)
    #error "D_EXT_INTERRUPT_LAST_SOURCE_USED is not defined"
#else
    #define D_PSP_EXT_INTERRUPT_LAST_SOURCE_USED     D_EXT_INTERRUPT_LAST_SOURCE_USED
#endif

/**
* macros
*/

#define M_PSP_IS_STANDARD_PRIORITY_ORDER ( \
  (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) \
  == D_PSP_EXT_INT_STANDARD_PRIORITY)

#define M_PSP_IS_REVERSED_PRIORITY_ORDER ( \
  (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) \
  == D_PSP_EXT_INT_REVERSED_PRIORITY)
/* 
* In the standard priority order, the available values of the priority
* are [1, 15] (including the range boundaries). The value 0 serves to "turn-off"
* all the interrupts.
*/
#define M_PSP_IS_IN_STANDARD_PRIORITY_RANGE(uiPriority) ( \
  ((D_PSP_EXT_INT_PRIORITY_1 <= (uiPriority)) && ((uiPriority) <= D_PSP_EXT_INT_PRIORITY_15)))

/* 
* In the reversed priority order, the available values of the priority
* are [14, 0] (including the range boundaries). The value 15 serves to "turn-off"
* all the interrupts.
*/
#define M_PSP_IS_IN_REVERSED_PRIORITY_RANGE(uiPriority) \
  ((uiPriority) <= D_PSP_EXT_INT_PRIORITY_14)

#define M_PSP_IS_PRIORITY_IN_RANGE(uiPriority) ( \
  (M_PSP_IS_STANDARD_PRIORITY_ORDER && M_PSP_IS_IN_STANDARD_PRIORITY_RANGE(uiPriority)) || \
  (M_PSP_IS_REVERSED_PRIORITY_ORDER && M_PSP_IS_IN_REVERSED_PRIORITY_RANGE(uiPriority)))

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

/* External interrupt handlers Global Table */
D_PSP_DATA_SECTION D_PSP_ALIGNED(1024) fptrPspInterruptHandler_t g_ExtInterruptHandlers[D_PSP_NUM_OF_HARTS][D_PSP_PIC_NUM_OF_EXT_INTERRUPTS];


/**
* functions
*/

/**
* @brief - Set external interrupts vector-table address at MEIVT CSR
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptSetVectorTableAddress(void* pExtIntVectTable)
{
  u32_t uiInterruptsState;

  /* Assert if vector-table address is NULL */
  M_PSP_ASSERT(NULL != pExtIntVectTable)

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  M_PSP_WRITE_CSR(D_PSP_MEIVT_NUM, pExtIntVectTable);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);
}

/*
* brief - This function registers external interrupt handler and delegate the handler to current Hart (HW thread)
*         i.e. when the given interrupt happens, the handler will run on the delegated hart (HW thread)
*
* @param uiVectorNumber = the number of the external interrupt to register
* @param pIsr = the ISR to register
* @param pParameter = NOT IN USE for baremetal implementation
* @return pOldIsr = pointer to the previously registered ISR
*/
D_PSP_TEXT_SECTION fptrPspInterruptHandler_t pspMachineExtInterruptRegisterISR(u32_t uiVectorNumber, fptrPspInterruptHandler_t pIsr, void* pParameter)
{
   fptrPspInterruptHandler_t fptrPrevIsr = NULL;
   u32_t uiHartNumber;
   u32_t uiInterruptsState;

   /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
   M_PSP_ASSERT((D_PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiVectorNumber)  && (D_PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiVectorNumber))

   /* Disable interrupts */
   pspMachineInterruptsDisable(&uiInterruptsState);

   /* Get the current Hart (HW thread) Id */
   uiHartNumber = M_PSP_MACHINE_GET_HART_ID();

   /* Set external-interrupts vector-table address in MEIVT CSR */
   pspMachineExtInterruptSetVectorTableAddress(&g_ExtInterruptHandlers[uiHartNumber]);

   /* Register the interrupt */
   fptrPrevIsr = g_ExtInterruptHandlers[uiHartNumber][uiVectorNumber];
   g_ExtInterruptHandlers[uiHartNumber][uiVectorNumber] = pIsr;

   /* Delegate the external-interrupt to current Hart (HW thread) */
   pspMachineExtInterruptDelegateToHart(uiVectorNumber, uiHartNumber);

   /* Restore interrupts */
   pspMachineInterruptsRestore(uiInterruptsState);

   return(fptrPrevIsr);
}

/**
* brief - This function disables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to disable
* @return None
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptDisableNumber(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
  M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiIntNum)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiIntNum))

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Clear Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + M_PSP_MULT_BY_4(uiIntNum)) , 0);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* brief - This function enables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to enable
* @return None
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptEnableNumber(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
  M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiIntNum)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiIntNum))

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + M_PSP_MULT_BY_4(uiIntNum)), D_PSP_MEIE_INT_EN_MASK);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* brief - This function sets the priority of a specified external interrupt
*
*  @param intNum = the number of the external interrupt to disable
*  @param priority = priority to be set
* @return None
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
  u32_t uiInterruptsState;

  /* Assertion of priority value */
  M_PSP_ASSERT(M_PSP_IS_PRIORITY_IN_RANGE(uiPriority));

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set priority in meipl register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_MEIPL_ADDR + M_PSP_MULT_BY_4(uiIntNum)), uiPriority);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* brief - This function sets the priority threshold of the external interrupts in the PIC
*
* @param threshold = priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptsSetThreshold(u32_t uiThreshold)
{
  u32_t uiInterruptsState;

  /* Assert on threshold value */
  M_PSP_ASSERT(uiThreshold <= D_PSP_EXT_INT_THRESHOLD_15);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set in meipt CSR, the priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEIPT_NUM, uiThreshold);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);
}

/*
* brief - This function sets the nesting priority threshold of the external interrupts in the PIC
*
* @param threshold = nesting priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptsSetNestingPriorityThreshold(u32_t uiNestingPriorityThreshold)
{
  u32_t uiInterruptsState;

  /* Assert on threshold value */
  M_PSP_ASSERT(uiNestingPriorityThreshold <= D_PSP_EXT_INT_THRESHOLD_15);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set in meicidpl CSR, the nesting priority priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEICIDPL_NUM, uiNestingPriorityThreshold);

  /* Set in meicurpl CSR, the nesting priority priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEICURPL_NUM, uiNestingPriorityThreshold);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);
}

/*
* brief - This function checks whether a given external interrupt is pending or not
*
* @param uiExtInterrupt = Number of external interrupt
* @return = pending (1) or not (0)
*/
D_PSP_TEXT_SECTION u32_t pspMachineExtInterruptIsPending(u32_t uiExtInterrupt)
{
  u32_t uiRegister, uiBit, uiResult;

  /* Calculate the meipX register to access to check the input interrupt number */
  uiRegister = D_PSP_MEIP_ADDR + D_PSP_REG32_BYTE_WIDTH * (uiExtInterrupt >> D_PSP_SHIFT_5);

  /* Calculate the bit in meipX register to access to check the input interrupt number */
  uiBit = uiExtInterrupt - (uiRegister * D_PSP_REG32_BIT_WIDTH);

  /* Check the specific bit */
  uiResult = !!( M_PSP_READ_REGISTER_32(uiRegister) & (1 << uiBit) );

  return (uiResult);
}


/*
* brief - This function sets external-interrupt type (Level-triggered or Edge-triggered)
*
* @param uiIntNum  = Number of external interrupt
* @param uiIntType = Type of the interrupt (level or edge)
*
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptSetType(u32_t uiIntNum, u32_t uiIntType)
{
  u32_t uiInterruptsState;

  /* Assert on interrupt-type value */
  M_PSP_ASSERT((D_PSP_EXT_INT_LEVEL_TRIG_TYPE == uiIntType) || (D_PSP_EXT_INT_EDGE_TRIG_TYPE == uiIntType));

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set interrupt type */
  M_PSP_SET_REGISTER_32(D_PSP_PIC_MEIGWCTRL_ADDR + M_PSP_MULT_BY_4(uiIntNum), uiIntType << D_PSP_MEIGWCTRL_TYPE_BIT_OFFSET);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}


/*
* brief - This function sets external-interrupt polarity (active-high or active-low)
*
* @param uiIntNum   = Number of external interrupt
* @param uiPolarity = active-high or active-low
*
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptSetPolarity(u32_t uiIntNum, u32_t uiPolarity)
{
  u32_t uiInterruptsState;

  /* Assert on interrupt-type value */
  M_PSP_ASSERT((D_PSP_EXT_INT_ACTIVE_HIGH == uiPolarity) || (D_PSP_EXT_INT_ACTIVE_LOW == uiPolarity));

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set interrupt type */
  M_PSP_SET_REGISTER_32(D_PSP_PIC_MEIGWCTRL_ADDR + M_PSP_MULT_BY_4(uiIntNum), uiPolarity << D_PSP_MEIGWCTRL_POLARITY_BIT_OFFSET);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* brief - This function clears the indication of pending interrupt
*
* @param uiIntNum   = Number of external interrupt
*/
D_PSP_TEXT_SECTION void  pspMachineExtInterruptClearPendingInt(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Clear the gwateway */
  M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MEIGWCLR_ADDR + M_PSP_MULT_BY_4(uiIntNum), 0);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* brief - This function sets Priority Order (Standard or Reserved)
*
* @param uiPriorityOrder = Standard or Reserved
*
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptSetPriorityOrder(u32_t uiPriorityOrder)
{
  u32_t uiInterruptsState;

  /* Assert on priority-order value */
  M_PSP_ASSERT((D_PSP_EXT_INT_STANDARD_PRIORITY == uiPriorityOrder) || (D_PSP_EXT_INT_REVERSED_PRIORITY == uiPriorityOrder));

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Set Priority Order */
  M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR, uiPriorityOrder);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* @brief - This function gets the current selected external interrupt (claim-id)
*
* @return - claim-id number
*/
D_PSP_TEXT_SECTION u32_t pspMachineExtInterruptGetClaimId(void)
{
  u32_t uiClaimId = (M_PSP_READ_CSR(D_PSP_MEIHAP_NUM) & D_PSP_MEIHAP_CLAIMID_MASK)  ;
  uiClaimId = uiClaimId >> D_PSP_SHIFT_2;

  return uiClaimId;
}


/*
* @brief - This function gets the priority of currently selected external interrupt
*
* @return - priority level
*/
D_PSP_TEXT_SECTION u32_t pspMachineExtInterruptGetPriority(void )
{
  u32_t uiPriorityLevel = (M_PSP_READ_CSR(D_PSP_MEICIDPL_NUM) & D_PSP_MEICIDPL_CLIDPRI_MASK) ;

  return uiPriorityLevel;
}

/*
* @brief - External interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptIsr(void)
{
  fptrFunction_t fptrExtIntHandler = NULL;
  u32_t* pClaimId;

  /* Trigger capture of the interrupt source ID(handler address), write '1' to meicpct */
  M_PSP_WRITE_CSR(D_PSP_MEICPCT_NUM, D_PSP_MEICPCT_CAPTURE_MASK);

  /* Obtain external interrupt handler address from meihap register */
  pClaimId = (u32_t*)M_PSP_READ_CSR(D_PSP_MEIHAP_NUM);

  fptrExtIntHandler = *((fptrFunction_t)pClaimId);

  M_PSP_ASSERT(fptrExtIntHandler != NULL);

  fptrExtIntHandler();
}

/**
* @brief - Delegate external interrupt to a Hart (HW thread)
*          When delegated, the given external-interrupt number will be visible only to the given hart
* 
* @parameter - external interrupt number
* @parameter - Hart number
*/
D_PSP_TEXT_SECTION void pspMachineExtInterruptDelegateToHart(u32_t uiExtIntNumber, u32_t uiHartNumber)
{
  u32_t uiInterruptsState;

  /* Assert on Hart number */
  M_PSP_ASSERT(E_LAST_HART > uiHartNumber);
  
  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsState);

  /* Delegate an external interrupt to a Hart*/
  M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MEIDEL_ADDR + M_PSP_MULT_BY_4(uiExtIntNumber), uiHartNumber);

  /* Restore interrupts */
  pspMachineInterruptsRestore(uiInterruptsState);

  /* Remove the multi-harts access protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS);
}

/*
* @brief - This function checks whether a given external interrupt is pending or not per the current Hart (HW thread)
* Via this api, only the external-interrupts that are delegated to current hart, could be seen, whether they are pending or not.
*
* @param uiExtInterrupt = Number of external interrupt
* @return = pending (1) or not (0)
*/
D_PSP_TEXT_SECTION u32_t pspMachineExtInterruptIsPendingOnHart(u32_t uiExtInterruptNumber)
{
  u32_t uiRegister, uiBit, uiResult;

  /* Calculate the meitpX register to access to check the input interrupt number */
  uiRegister = D_PSP_MEITP_ADDR + D_PSP_REG32_BYTE_WIDTH * (uiExtInterruptNumber >> D_PSP_SHIFT_5);

  /* Calculate the bit in meitpX register to access to check the input interrupt number */
  uiBit = uiExtInterruptNumber - (uiRegister * D_PSP_REG32_BIT_WIDTH);

  /* Check the specific bit */
  uiResult = !!( M_PSP_READ_REGISTER_32(uiRegister) & (1 << uiBit) );

  return (uiResult);
}
