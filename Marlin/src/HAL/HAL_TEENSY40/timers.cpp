/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Teensy4.0 (__IMXRT1062__)
 */

#if defined(__IMXRT1062__)

#if defined(WARN_UNTESTED)
#warning Teensy4.0 > Timers: This code has not been tested yet
#endif

#include "HAL.h"
#include "timers.h"

/** \brief Instruction Synchronization Barrier
  Instruction Synchronization Barrier flushes the pipeline in the processor,
  so that all instructions following the ISB are fetched from cache or
  memory, after the instruction has been completed.
*/
FORCE_INLINE static void __ISB() {
  __asm__ __volatile__("isb 0xF":::"memory");
}

/** \brief Data Synchronization Barrier
  This function acts as a special kind of Data Memory Barrier.
  It completes when all explicit memory accesses before this instruction complete.
*/
FORCE_INLINE static void __DSB() {
  __asm__ __volatile__("dsb 0xF":::"memory");
}

void isr_pit() {
  if (PIT_TFLG1 & PIT_TFLG_TIF) proxy_isr_pit1();
  if (PIT_TFLG3 & PIT_TFLG_TIF) proxy_isr_pit3();
}

void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency) {
  static bool init = false;
  if (!init) {
    PIT_MCR = 0x00;
    init = true;
    attachInterruptVector(IRQ_PIT, &isr_pit);
    NVIC_SET_PRIORITY(IRQ_PIT, 16);
  }

  switch (timer_num) {
    case 0:
      PIT_LDVAL1 = F_24M / frequency;
      PIT_TCTRL1 |= PIT_TCTRL_TEN;
      break;
    case 1:
      PIT_LDVAL3 = F_24M / frequency;
      PIT_TCTRL3 |= PIT_TCTRL_TEN;
      break;
  }
}

void HAL_timer_enable_interrupt(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      PIT_TCTRL1 |= PIT_TCTRL_TIE;
      break;
    case 1:
      PIT_TCTRL3 |= PIT_TCTRL_TIE;
      break;
  }
  
  int any = PIT_TCTRL1 & PIT_TCTRL3 & PIT_TCTRL_TIE;
  if (any) NVIC_ENABLE_IRQ(IRQ_PIT);
  
}

void HAL_timer_disable_interrupt(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      PIT_TCTRL1 &= ~PIT_TCTRL_TIE;
      break;
    case 1:
      PIT_TCTRL3 &= ~PIT_TCTRL_TIE;
      break;
  }
  int any = PIT_TCTRL1 & PIT_TCTRL3 & PIT_TCTRL_TIE;
  if (!any) NVIC_DISABLE_IRQ(IRQ_PIT);
  
  // We NEED memory barriers to ensure Interrupts are actually disabled!
  // ( https://dzone.com/articles/nvic-disabling-interrupts-on-arm-cortex-m-and-the )
  __DSB();
  __ISB();
}

bool HAL_timer_interrupt_enabled(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      return NVIC_IS_ENABLED(IRQ_PIT) && (PIT_TCTRL1 & PIT_TCTRL_TIE);
    case 1:
      return NVIC_IS_ENABLED(IRQ_PIT) && (PIT_TCTRL3 & PIT_TCTRL_TIE);
  }
  return false;
}

void HAL_timer_isr_prologue(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      PIT_TFLG1 |= PIT_TFLG_TIF;
      break;
    case 1:
      PIT_TFLG3 |= PIT_TFLG_TIF;
      break;
  }
}

#endif // Teensy4.0
