/*
 SPDX-License-Identifier: Apache-2.0
 Copyright 2019 - 2021 Western Digital Corporation or its affiliates.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http:www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

/**
* @file   psp_macros.inc
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file defines psp macros that are used in Assembly code. These macros depend on the core definition of 32/64 bits
*/


/* Macros that depend on whether the core is 32bits or 64bits */
.if __riscv_xlen == 32
    .equ REGBYTES, 4

    .macro M_PSP_STORE operand1,operand2
        sw \operand1, \operand2
    .endm

    .macro M_PSP_LOAD operand1,operand2
        lw \operand1, \operand2
    .endm

    .macro M_PSP_ADDI operand1,operand2,operand3
        addi \operand1, \operand2, \operand3
    .endm

.elseif __riscv_xlen == 64
    .error "You must verify it is work correctly for 64 bit"

    .equ REGBYTES, 8

    .macro M_PSP_STORE operand1,operand2
        sd \operand1, \operand2
    .endm

    .macro M_PSP_LOAD operand1,operand2
        ld \operand1, \operand2
    .endm

    .macro M_PSP_ADDI operand1,operand2,operand3
        addiw \operand1, \operand2, \operand3
    .endm
.endif

.macro M_PSP_STORE_BYTE operand1,operand2
    sb \operand1, \operand2
.endm

.macro M_PSP_LOAD_BYTE operand1,operand2
    lb \operand1, \operand2
.endm

.macro M_PSP_STORE_HALF operand1,operand2
    sh \operand1, \operand2
.endm

.macro M_PSP_LOAD_HALF operand1,operand2
    lh \operand1, \operand2
.endm


.equ D_PSP_MIE, 8

/* Macro for calling interrupt handler */
.macro M_PSP_CALL_INT_HANDLER fptIntHandler
    /* load the address of provided fptIntHandler */
    la        a0, \fptIntHandler
    /* load the actual handler address */
    M_PSP_LOAD    a0, 0x0(a0)
    /* invoke the interrupt handler */
    jalr      a0
.endm

/* Macro for saving application context */
.macro M_PSP_PUSH_REGFILE
    /* make room in stack for the starndard core registers.
     * In addition, make sure SP is aligned to 16 */
    M_PSP_ADDI    sp, sp, -REGBYTES * 32
    /* Save Context */
    M_PSP_STORE    x1,   3  * REGBYTES(sp)
    M_PSP_STORE    x5,   4  * REGBYTES(sp)
    M_PSP_STORE    x6,   5  * REGBYTES(sp)
    M_PSP_STORE    x7,   6  * REGBYTES(sp)
    M_PSP_STORE    x8,   7  * REGBYTES(sp)
    M_PSP_STORE    x9,   8  * REGBYTES(sp)
    M_PSP_STORE    x10,  9  * REGBYTES(sp)
    M_PSP_STORE    x11, 10  * REGBYTES(sp)
    M_PSP_STORE    x12, 11  * REGBYTES(sp)
    M_PSP_STORE    x13, 12  * REGBYTES(sp)
    M_PSP_STORE    x14, 13  * REGBYTES(sp)
    M_PSP_STORE    x15, 14  * REGBYTES(sp)
    M_PSP_STORE    x16, 15  * REGBYTES(sp)
    M_PSP_STORE    x17, 16  * REGBYTES(sp)
    M_PSP_STORE    x18, 17  * REGBYTES(sp)
    M_PSP_STORE    x19, 18  * REGBYTES(sp)
    M_PSP_STORE    x20, 19  * REGBYTES(sp)
    M_PSP_STORE    x21, 20  * REGBYTES(sp)
    M_PSP_STORE    x22, 21  * REGBYTES(sp)
    M_PSP_STORE    x23, 22  * REGBYTES(sp)
    M_PSP_STORE    x24, 23  * REGBYTES(sp)
    M_PSP_STORE    x25, 24  * REGBYTES(sp)
    M_PSP_STORE    x26, 25  * REGBYTES(sp)
    M_PSP_STORE    x27, 26  * REGBYTES(sp)
    M_PSP_STORE    x28, 27  * REGBYTES(sp)
    M_PSP_STORE    x30, 29  * REGBYTES(sp)
#ifndef D_RESERVE_REGS_29_31
    M_PSP_STORE    x29, 28  * REGBYTES(sp)
    M_PSP_STORE    x31, 30  * REGBYTES(sp)
#endif /* D_RESERVE_REGS_29_31 */
#ifdef D_RESERVE_TP
    M_PSP_STORE    x4,  28  * REGBYTES(sp)
#endif /* D_RESERVE_TP */
.endm

/* Macro for restoring application context */
.macro M_PSP_POP_REGFILE
    /* Restore core standard registers,
    Skip global pointer because that does not change */
    M_PSP_LOAD    x1,   3 * REGBYTES(sp)
    M_PSP_LOAD    x5,   4 * REGBYTES(sp)
    M_PSP_LOAD    x6,   5 * REGBYTES(sp)
    M_PSP_LOAD    x7,   6 * REGBYTES(sp)
    M_PSP_LOAD    x8,   7 * REGBYTES(sp)
    M_PSP_LOAD    x9,   8 * REGBYTES(sp)
    M_PSP_LOAD    x10,  9 * REGBYTES(sp)
    M_PSP_LOAD    x11, 10 * REGBYTES(sp)
    M_PSP_LOAD    x12, 11 * REGBYTES(sp)
    M_PSP_LOAD    x13, 12 * REGBYTES(sp)
    M_PSP_LOAD    x14, 13 * REGBYTES(sp)
    M_PSP_LOAD    x15, 14 * REGBYTES(sp)
    M_PSP_LOAD    x16, 15 * REGBYTES(sp)
    M_PSP_LOAD    x17, 16 * REGBYTES(sp)
    M_PSP_LOAD    x18, 17 * REGBYTES(sp)
    M_PSP_LOAD    x19, 18 * REGBYTES(sp)
    M_PSP_LOAD    x20, 19 * REGBYTES(sp)
    M_PSP_LOAD    x21, 20 * REGBYTES(sp)
    M_PSP_LOAD    x22, 21 * REGBYTES(sp)
    M_PSP_LOAD    x23, 22 * REGBYTES(sp)
    M_PSP_LOAD    x24, 23 * REGBYTES(sp)
    M_PSP_LOAD    x25, 24 * REGBYTES(sp)
    M_PSP_LOAD    x26, 25 * REGBYTES(sp)
    M_PSP_LOAD    x27, 26 * REGBYTES(sp)
    M_PSP_LOAD    x28, 27 * REGBYTES(sp)
    M_PSP_LOAD    x30, 29 * REGBYTES(sp)
#ifndef D_RESERVE_REGS_29_31
    M_PSP_LOAD    x29, 28 * REGBYTES(sp)
    M_PSP_LOAD    x31, 30 * REGBYTES(sp)
#endif /* D_RESERVE_REGS_29_31 */
#ifdef D_RESERVE_TP
    M_PSP_LOAD    x4,  28 * REGBYTES(sp)
#endif /* D_RESERVE_TP */
    /* Make sure SP is aligned to 16 */
    M_PSP_ADDI    sp, sp, REGBYTES * 32


.endm

/* Macro for setting SP to use stack dedicated to ISRs */
/* [NR] - To do: add stack check */
.macro M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK pIsrStack
    mv t0, sp
    M_PSP_LOAD    sp, \pIsrStack
    M_PSP_ADDI    sp, sp, -REGBYTES * 4
    M_PSP_STORE   t0, 0 * REGBYTES(sp)
.endm

/* Macro for restoring application stack pointer. */
.macro M_PSP_RESTORE_APP_SP
    M_PSP_LOAD    sp, 0 * REGBYTES(sp)
.endm

/* Macro for disable machine interrupts after reading the current status . Available in MACHINE mode only */
.macro M_PSP_MACHINE_DISABLE_INTERRUPTS saveCurrentStatusToReg
    csrrci \saveCurrentStatusToReg, mstatus, D_PSP_MIE
.endm

/* Macro for restore machine interrupts status . Available in MACHINE mode only */
.macro M_PSP_MACHINE_RESTORE_INTERRUPTS restorStatusFromReg
    csrs mstatus, \restorStatusFromReg
.endm

