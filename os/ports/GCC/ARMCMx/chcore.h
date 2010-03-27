/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    ARMCMx/chcore.h
 * @brief   ARM Cortex-Mx architecture port macros and structures.
 *
 * @addtogroup ARMCMx_CORE
 * @{
 */

#ifndef _CHCORE_H_
#define _CHCORE_H_

/*
 * Port-related configuration parameters.
 */

/**
 * @brief   Enables the use of the WFI ins.
 */
#ifndef ENABLE_WFI_IDLE
#define ENABLE_WFI_IDLE         0
#endif

#define CORTEX_M0               0       /**< @brief Cortex-M0 variant.      */
#define CORTEX_M3               3       /**< @brief Cortex-M3 variant.      */

/**
 * @brief   Name of the implemented architecture.
 */
#define CH_ARCHITECTURE_NAME "ARM"

/* Inclusion of the Cortex-Mx implementation specific parameters.*/
#include "cmparams.h"

/* Generating model-dependent info.*/
#if (CORTEX_MODEL == CORTEX_M3) || defined(__DOXYGEN__)
/**
 * @brief   Macro defining the ARM Cortex-M3 architecture.
 */
#define CH_ARCHITECTURE_ARMCM3

/**
 * @brief   Name of the architecture variant (optional).
 */
#define CH_CORE_VARIANT_NAME    "Cortex-M3"
#elif (CORTEX_MODEL == CORTEX_M0)
#define CH_ARCHITECTURE_ARMCM0
#define CH_CORE_VARIANT_NAME    "Cortex-M0"
#else
#error "unknown or unsupported Cortex-M model"
#endif

/**
 * @brief   32 bits stack and memory alignment enforcement.
 */
typedef uint32_t stkalign_t;

/**
 * @brief   Generic ARM register.
 */
typedef void *regarm_t;

#if !defined(__DOXYGEN__)
/**
 * @brief   Interrupt saved context.
 * @details This structure represents the stack frame saved during a
 *          preemption-capable interrupt handler.
 * @note    This structure is empty in this port.
 */
struct extctx {
};
#endif

#if !defined(__DOXYGEN__)
/**
 * @brief   System saved context.
 * @details This structure represents the inner stack frame during a context
 *          switching.
 */
struct intctx {
  regarm_t      basepri;
  regarm_t      r4;
  regarm_t      r5;
  regarm_t      r6;
#ifndef CH_CURRP_REGISTER_CACHE
  regarm_t      r7;
#endif
  regarm_t      r8;
  regarm_t      r9;
  regarm_t      r10;
  regarm_t      r11;
  regarm_t      lr_exc;
  /* Start of the hardware saved frame.*/
  regarm_t      r0;
  regarm_t      r1;
  regarm_t      r2;
  regarm_t      r3;
  regarm_t      r12;
  regarm_t      lr_thd;
  regarm_t      pc;
  regarm_t      xpsr;
};
#endif

#if !defined(__DOXYGEN__)
/**
 * @brief   Platform dependent part of the @p Thread structure.
 * @details In the Cortex-Mx port this structure just holds a pointer to the
 *          @p intctx structure representing the stack pointer at the time
 *          of the context switch.
 */
struct context {
  struct intctx *r13;
};
#endif

/**
 * @brief   Platform dependent part of the @p chThdInit() API.
 * @details This code usually setup the context switching frame represented
 *          by an @p intctx structure.
 */
#define SETUP_CONTEXT(workspace, wsize, pf, arg) {                      \
  tp->p_ctx.r13 = (struct intctx *)((uint8_t *)workspace +              \
                                     wsize -                            \
                                     sizeof(struct intctx));            \
  tp->p_ctx.r13->basepri = CORTEX_BASEPRI_USER;                         \
  tp->p_ctx.r13->lr_exc = (regarm_t)0xFFFFFFFD;                         \
  tp->p_ctx.r13->r0 = arg;                                              \
  tp->p_ctx.r13->lr_thd = chThdExit;                                    \
  tp->p_ctx.r13->pc = pf;                                               \
  tp->p_ctx.r13->xpsr = (regarm_t)0x01000000;                           \
}

/**
 * @brief   Stack size for the system idle thread.
 * @details This size depends on the idle thread implementation, usually
 *          the idle thread should take no more space than those reserved
 *          by @p INT_REQUIRED_STACK.
 * @note    In this port it is set to 4 because the idle thread does have
 *          a stack frame when compiling without optimizations.
 */
#ifndef IDLE_THREAD_STACK_SIZE
#define IDLE_THREAD_STACK_SIZE      4
#endif

/**
 * @brief   Per-thread stack overhead for interrupts servicing.
 * @details This constant is used in the calculation of the correct working
 *          area size.
 *          This value can be zero on those architecture where there is a
 *          separate interrupt stack and the stack space between @p intctx and
 *          @p extctx is known to be zero.
 * @note    This port requires no extra stack space for interrupt handling.
 */
#ifndef INT_REQUIRED_STACK
#define INT_REQUIRED_STACK          0
#endif

/**
 * @brief   Enforces a correct alignment for a stack area size value.
 */
#define STACK_ALIGN(n) ((((n) - 1) | (sizeof(stkalign_t) - 1)) + 1)

/**
 * @brief   Computes the thread working area global size.
 */
#define THD_WA_SIZE(n) STACK_ALIGN(sizeof(Thread) +                     \
                                   sizeof(struct intctx) +              \
                                   sizeof(struct extctx) +              \
                                   (n) + (INT_REQUIRED_STACK))

/**
 * @brief   Static working area allocation.
 * @details This macro is used to allocate a static thread working area
 *          aligned as both position and size.
 */
#define WORKING_AREA(s, n) stkalign_t s[THD_WA_SIZE(n) / sizeof(stkalign_t)];

/**
 * @brief   IRQ prologue code.
 * @details This macro must be inserted at the start of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_PROLOGUE()

/**
 * @brief   IRQ epilogue code.
 * @details This macro must be inserted at the end of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_EPILOGUE() {                                           \
  chSysLockFromIsr();                                                   \
  if (chSchIsRescRequiredI())                                           \
    SCB_ICSR = ICSR_PENDSVSET;                                          \
  chSysUnlockFromIsr();                                                 \
}

/**
 * @brief   IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#define PORT_IRQ_HANDLER(id) void id(void)

/**
 * @brief   Port-related initialization code.
 * @note    This function is empty in this port.
 */
#define port_init()

/**
 * @brief   Kernel-lock action.
 * @details Usually this function just disables interrupts but may perform
 *          more actions.
 * @note    In this port this it raises the base priority to kernel level.
 */
#if CH_OPTIMIZE_SPEED
#define port_lock() {                                                   \
  register uint32_t tmp asm ("r3") = CORTEX_BASEPRI_KERNEL;             \
  asm volatile ("msr     BASEPRI, %0" : : "r" (tmp));                   \
}
#else
#define port_lock() {                                                   \
  asm volatile ("bl      _port_lock" : : : "r3", "lr");                 \
}
#endif

/**
 * @brief   Kernel-unlock action.
 * @details Usually this function just disables interrupts but may perform
 *          more actions.
 * @note    In this port this it lowers the base priority to kernel level.
 */
#if CH_OPTIMIZE_SPEED
#define port_unlock() {                                                 \
  register uint32_t tmp asm ("r3") = CORTEX_BASEPRI_USER;               \
  asm volatile ("msr     BASEPRI, %0" : : "r" (tmp));                   \
}
#else
#define port_unlock() {                                                 \
  asm volatile ("bl      _port_unlock" : : : "r3", "lr");               \
}
#endif

/**
 * @brief   Kernel-lock action from an interrupt handler.
 * @details This function is invoked before invoking I-class APIs from
 *          interrupt handlers. The implementation is architecture dependent,
 *          in its simplest form it is void.
 * @note    Same as @p port_lock() in this port.
 */
#define port_lock_from_isr() port_lock()

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 * @details This function is invoked after invoking I-class APIs from interrupt
 *          handlers. The implementation is architecture dependent, in its
 *          simplest form it is void.
 * @note    Same as @p port_unlock() in this port.
 */
#define port_unlock_from_isr() port_unlock()

/**
 * @brief   Disables all the interrupt sources.
 * @note    Of course non maskable interrupt sources are not included.
 * @note    In this port it disables all the interrupt sources by raising
 *          the priority mask to level 0.
 */
#define port_disable() asm volatile ("cpsid   i")

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 * @note    Interrupt sources above kernel level remains enabled.
 * @note    In this port it raises/lowers the base priority to kernel level.
 */
#define port_suspend() {                                                \
  register uint32_t tmp asm ("r3") = CORTEX_BASEPRI_KERNEL;             \
  asm volatile ("msr     BASEPRI, %0                    \n\t"           \
                "cpsie   i" : : "r" (tmp));                             \
}

/**
 * @brief   Enables all the interrupt sources.
 * @note    In this port it lowers the base priority to user level.
 */
#define port_enable() {                                                 \
  register uint32_t tmp asm ("r3") = CORTEX_BASEPRI_USER;               \
  asm volatile ("msr     BASEPRI, %0                    \n\t"           \
                "cpsie   i" : : "r" (tmp));                             \
}

/**
 * @brief   Enters an architecture-dependent IRQ-waiting mode.
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 * @note    Implemented as an inlined @p WFI instruction.
 */
#if ENABLE_WFI_IDLE || defined(__DOXYGEN__)
#define port_wait_for_interrupt() {                                     \
  asm volatile ("wfi");                                                 \
}
#else
#define port_wait_for_interrupt()
#endif

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 * @note    Implemented as inlined code for performance reasons.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 */
static INLINE Thread *port_switch(Thread *ntp, Thread *otp) {
  register Thread *_ntp asm ("r0") = (ntp);
  register Thread *_otp asm ("r1") = (otp);
#if CH_DBG_ENABLE_STACK_CHECK
  register char *sp asm ("sp");
  if (sp - sizeof(struct intctx) - sizeof(Thread) < (char *)_otp)
    asm volatile ("movs    r0, #0                               \n\t"
                  "b       chDbgPanic");
#endif /* CH_DBG_ENABLE_STACK_CHECK */
  asm volatile ("svc     #0" : : "r" (_otp), "r" (_ntp) : "memory");
  return _otp;
}

#ifdef __cplusplus
extern "C" {
#endif
  void port_halt(void);
#if !CH_OPTIMIZE_SPEED
  void _port_lock(void);
  void _port_unlock(void);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _CHCORE_H_ */

/** @} */