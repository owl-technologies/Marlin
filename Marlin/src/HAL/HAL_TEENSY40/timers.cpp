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
#include "../../core/serial.h"
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

void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency) {
  switch (timer_num) {
    case 0:
        CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) ;  // enable GPT1 module
        GPT1_CR = 0;
        GPT1_PR = 0;   // prescale+1
        GPT1_OCR1 = HAL_TIMER_RATE / frequency - 1;  // compare
        GPT1_SR = 0x3F; // clear all prior status
        GPT1_IR = GPT_IR_OF1IE;
        GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;// 1 ipg 24mhz   
        attachInterruptVector(IRQ_GPT1, step_isr_pit1);
//        NVIC_SET_PRIORITY(IRQ_GPT1, 1);
      break;
    case 1:
        CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) ;
        GPT2_CR = 0;
        GPT2_PR = 0;   // prescale+1
        GPT2_OCR1 = HAL_TIMER_RATE / frequency - 1;  // compare
        GPT2_SR = 0x3F; // clear all prior status
        GPT2_IR = GPT_IR_OF1IE;
        GPT2_CR = GPT_CR_EN | GPT_CR_CLKSRC(1) ;// 1 ipg 24mhz   
        attachInterruptVector(IRQ_GPT2, temp_isr_pit3);
      break;
  }
}

void HAL_timer_enable_interrupt(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      NVIC_ENABLE_IRQ(IRQ_GPT1);
      break;
    case 1:
      NVIC_ENABLE_IRQ(IRQ_GPT2);
      break;
  }  
}

void HAL_timer_disable_interrupt(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      NVIC_DISABLE_IRQ(IRQ_GPT1);
      break;
    case 1:
      NVIC_DISABLE_IRQ(IRQ_GPT2);
      break;
  }
  
  // We NEED memory barriers to ensure Interrupts are actually disabled!
  // ( https://dzone.com/articles/nvic-disabling-interrupts-on-arm-cortex-m-and-the )
  __DSB();
  __ISB();
}

bool HAL_timer_interrupt_enabled(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
      return NVIC_IS_ENABLED(IRQ_GPT1);
    case 1:
      return NVIC_IS_ENABLED(IRQ_GPT2);
  }
  return false;
}

void HAL_timer_isr_prologue(const uint8_t timer_num) {
  switch (timer_num) {
    case 0:
        GPT1_SR |= GPT_SR_OF1;  // clear all set bits
        // while (GPT1_SR & GPT_SR_OF3); // wait for clear
      break;
    case 1:
      GPT2_SR |= GPT_SR_OF1;
      break;
  }
}

#endif // Teensy4.0
