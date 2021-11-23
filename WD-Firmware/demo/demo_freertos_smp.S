
#include "psp_macros.inc"
#include "rtosal_defines.inc"
#include "rtosal_macros.inc"

#ifdef D_LLVM_NO_WARN_RESERVED_REGS
/* disable warning for reserved registers use - we are using comrv
   reserved register and don't want to see these warnings. */
.option nowarnreservedreg
#endif /* D_LLVM_NO_WARN_RESERVED_REGS */

.align 4
.global    rtosalHandleExternal

.align 8
/* Implements context-switch as a result of triggered external interrupt */
rtosalHandleExternal:
    M_PSP_PUSH_REGFILE                                       /* Push registers of current application onto stack */
    M_RTOSAL_READ_HART_ID a2
    M_RTOSAL_SET_INT_CONTEXT a2                              /* Mark we are in interrupt context */
  .if D_ENABLE_FREERTOS_SMP == 1
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCBs, 0, a2                /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
  .else
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0                    /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
  .endif /* D_ENABLE_FREERTOS_SMP */
    M_RTOSAL_CHANGE_SP_FROM_APP_TO_ISR_STACK xISRStackMultiHart, a2                 /* After RegFile is pushed onto application's-stack, we change sp to point to ISR-Stack */
    /* pspMachineExtInterruptGetClaimId return value in a0 is passed to bspClearExtInterrupt */
    jal pspMachineExtInterruptGetClaimId
    jal bspClearExtInterrupt
    M_RTOSAL_SWITCH_CONTEXT
    M_RTOSAL_READ_HART_ID a2
  .if D_ENABLE_FREERTOS_SMP == 1
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCBs, 0, a2            /* Restore SP (application's one) and MEPC & MSTATUS */
  .else
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0                 /* Restore SP (application's one) and MEPC & MSTATUS */
  .endif /* D_ENABLE_FREERTOS_SMP */
    M_RTOSAL_CLEAR_INT_CONTEXT a2                            /* Clear interrupt context indication */
    M_PSP_POP_REGFILE
    mret
