/* 
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019-2021 Western Digital Corporation or its affiliates.
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
* @file   comrv.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The defines COM-RV private interfaces
* 
*/
#ifndef  __COMRV_H__
#define  __COMRV_H__

/**
* include files
*/

/**
* definitions
*/

/* mstatus CSR */
#define D_PSP_MSTATUS_NUM          0x300
#define D_PSP_MSTATUS_UIE_MASK     0x00000001   /* User mode */
#define D_PSP_MSTATUS_SIE_MASK     0x00000002   /* Supervisor mode */
#define D_PSP_MSTATUS_MIE_MASK     0x00000008   /* Machine mode */

/**
* macros
*/
/* __builtin_expect instruction provides branch
   prediction information. The condition parameter is the expected
   comparison value. If it is equal to 1 (true), the condition
   is likely to be true, in other case condition is likely to be false.
   this provides us a way to take rare cases out of the critical execution path */
#define M_PSP_BUILTIN_EXPECT(condition, expected)  __builtin_expect(condition, expected)

/* CSR clear bits and read */
#define _CLEAR_AND_READ_CSR_(read_val, reg, bits) ({ \
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(read_val) : "i"(bits)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(read_val) : "r"(bits)); \
  read_val; })
#define _CLEAR_AND_READ_CSR_INTERMEDIATE_(read_val, reg, bits) _CLEAR_AND_READ_CSR_(read_val, reg, bits)
#define M_PSP_CLEAR_AND_READ_CSR(read_val, csr, bits) _CLEAR_AND_READ_CSR_INTERMEDIATE_(read_val, csr, bits)

/* CSR set */
#define _SET_CSR_(reg, bits) ({\
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrs " #reg ", %0" :: "i"(bits)); \
  else \
    asm volatile ("csrs " #reg ", %0" :: "r"(bits)); })
#define _SET_CSR_INTERMEDIATE_(reg, bits) _SET_CSR_(reg, bits)
#define M_PSP_SET_CSR(csr, bits)                      _SET_CSR_INTERMEDIATE_(csr, bits)

#define D_COMRV_TEXT_SECTION     __attribute__((section("COMRV_TEXT_SEC")))
#define D_COMRV_DATA_SECTION     __attribute__((section("COMRV_DATA_SEC")))
#define D_COMRV_RODATA_SECTION   __attribute__((section("COMRV_RODATA_SEC")))
#ifdef D_COMRV_CODE_SIZE_SUPPORT
#define D_COMRV_NO_CHANGE_IN_FLOW_TEXT_SECTION     __attribute__((section("COMRV_TEXT_NO_CHANGE_IN_FLOW_SEC")))
#else
#define D_COMRV_NO_CHANGE_IN_FLOW_TEXT_SECTION
#endif /* D_COMRV_CODE_SIZE_SUPPORT */

#ifdef D_COMRV_ERROR_NOTIFICATIONS
   #define M_COMRV_ERROR(stError,errorNum,token)   stError.uiErrorNum = errorNum; \
                                                   stError.uiToken    = token; \
                                                   comrvErrorHook(&stError);
#else
   #define M_COMRV_ERROR(stError,errorNum,token)
#endif /* D_COMRV_ERROR_NOTIFICATIONS */

#ifdef D_COMRV_ASSERT_ENABLED
   #ifdef D_COMRV_ERROR_NOTIFICATIONS
      #define M_COMRV_ASSERT_ACTION(error,tokenVal)      M_COMRV_ERROR(stErrArgs,error,tokenVal);
   #else
      #define M_COMRV_ASSERT_ACTION(error,tokenVal)      while(1);
   #endif /* D_COMRV_ERROR_NOTIFICATIONS */
   #define M_COMRV_ASSERT(expression, expectedVlue, error, tokenVal)  if (M_COMRV_BUILTIN_EXPECT((expression) != (expectedVlue), 0)) \
                                                                      { \
                                                                         M_COMRV_ASSERT_ACTION(error,tokenVal); \
                                                                      }
#else
   #define M_COMRV_ASSERT(expression, expectedVlue, error, tokenVal)
#endif /* D_COMRV_ASSERT_ENABLED */

#ifdef D_COMRV_RTOS_SUPPORT

   #define M_COMRV_DISABLE_INTS(uiOutPrevIntState) M_PSP_CLEAR_AND_READ_CSR(uiOutPrevIntState, D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK) );
   #define M_COMRV_ENABLE_INTS(uiOutPrevIntState)  M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiOutPrevIntState)
   #define M_COMRV_ENTER_CRITICAL_SECTION()  ret = comrvEnterCriticalSectionHook(); \
                                             M_COMRV_ASSERT(ret, D_COMRV_SUCCESS, D_COMRV_ENTER_CRITICAL_SECTION_ERR, unToken.uiValue);
   #define M_COMRV_EXIT_CRITICAL_SECTION()   ret = comrvExitCriticalSectionHook(); \
                                             M_COMRV_ASSERT(ret, D_COMRV_SUCCESS, D_COMRV_EXIT_CRITICAL_SECTION_ERR, unToken.uiValue);
#else
   #define M_COMRV_ENTER_CRITICAL_SECTION()
   #define M_COMRV_EXIT_CRITICAL_SECTION()
   #define M_COMRV_DISABLE_INTS(uiOutPrevIntState) (void)uiOutPrevIntState
   #define M_COMRV_ENABLE_INTS(uiOutPrevIntState)  (void)uiOutPrevIntState
#endif /* D_COMRV_RTOS_SUPPORT */

/* M_PSP_BUILTIN_EXPECT instruction provides branch
   prediction information. The condition parameter is the expected
   comparison value. If it is equal to 1 (true), the condition
   is likely to be true, in other case condition is likely to be false.
   this provides us a way to take rare cases out of the critical execution path */
#define M_COMRV_BUILTIN_EXPECT(condition, expected)  M_PSP_BUILTIN_EXPECT(condition, expected)

/* invalidate data cache */
#define M_COMRV_DCACHE_FLUSH(pAddress, uiNumOfBytes) comrvInvalidateDataCacheHook(pAddress, uiNumOfBytes);

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

#endif /* __COMRV_H__ */
