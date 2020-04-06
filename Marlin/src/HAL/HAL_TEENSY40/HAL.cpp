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
 * Description: HAL for Teensy40 (IMXRT1062)
 */

#if defined(__IMXRT1062__)

#include "HAL.h"
#include "../shared/Delay.h"

#include <Wire.h>

uint16_t HAL_adc_result, HAL_adc_select;

const uint8_t pin_to_channel[] = { // pg 482
	7,	// 0/A0  AD_B1_02
	8,	// 1/A1  AD_B1_03
	12,	// 2/A2  AD_B1_07
	11,	// 3/A3  AD_B1_06
	6,	// 4/A4  AD_B1_01
	5,	// 5/A5  AD_B1_00
	15,	// 6/A6  AD_B1_10
	0,	// 7/A7  AD_B1_11
	13,	// 8/A8  AD_B1_08
	14,	// 9/A9  AD_B1_09
	1,	// 24/A10 AD_B0_12 
	2,	// 25/A11 AD_B0_13
	128+3,	// 26/A12 AD_B1_14 - only on ADC2, 3
	128+4,	// 27/A13 AD_B1_15 - only on ADC2, 4
	7,	// 14/A0  AD_B1_02
	8,	// 15/A1  AD_B1_03
	12,	// 16/A2  AD_B1_07
	11,	// 17/A3  AD_B1_06
	6,	// 18/A4  AD_B1_01
	5,	// 19/A5  AD_B1_00
	15,	// 20/A6  AD_B1_10
	0,	// 21/A7  AD_B1_11
	13,	// 22/A8  AD_B1_08
	14,	// 23/A9  AD_B1_09
	1,	// 24/A10 AD_B0_12
	2,	// 25/A11 AD_B0_13
	128+3,	// 26/A12 AD_B1_14 - only on ADC2, 3
	128+4	// 27/A13 AD_B1_15 - only on ADC2, 4
};

/*
  // disable interrupts
  void cli() { noInterrupts(); }

  // enable interrupts
  void sei() { interrupts(); }
*/

void HAL_adc_init() {
  analog_init();
  
  #if 0
    // NOTE: already implied in analog.c, shouldn't be needed here 
    while (ADC1_GC & ADC_GC_CAL) {}; // Wait for calibration to finish
    while (ADC2_GC & ADC_GC_CAL) {}; // Wait for calibration to finish
  #endif

  #if 0
    // NOTE: Don't understand why this is enabled here instead of in timers
    NVIC_ENABLE_IRQ(IRQ_GPT2); // equiv of teensy 3.5 IRQ_FTM1
  #endif
  
}

// NOTE: Should probably be in watchdog for IMXRT1062
void HAL_clear_reset_source() { }
uint8_t HAL_get_reset_source() {
  switch (WDOG1_WRSR) {
    case WDOG_WRSR_POR: return RST_POWER_ON; break;
    case WDOG_WRSR_SFTW: return RST_EXTERNAL; break;
    case WDOG_WRSR_TOUT: return RST_WATCHDOG; break;
  }
  return 0;
}

extern "C" {
  extern char _sbss;
  extern char _ebss;
  extern char __heap_start;
  extern void* __brkval;

  int freeMemory() {
    int free_memory;
    if ((int)__brkval == 0)
      free_memory = ((int)&free_memory) - ((int)&_ebss);
    else
      free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
  }
}

void HAL_adc_start_conversion(const uint8_t adc_pin) {
  if (adc_pin > sizeof(pin_to_channel)) 
  {
    HAL_adc_select = -1;
    return;
  }
  uint8_t channel = pin_to_channel[adc_pin];
  if (channel & 0x80) {
    HAL_adc_select = 1;
    ADC2_HC0 = channel & 0x7F;
  }
  else {
    HAL_adc_select = 0;
    ADC1_HC0 = channel;
  }
}

uint16_t HAL_adc_get_result() {
  switch (HAL_adc_select) {
    case 0: 
      return ADC1_R0;

    case 1: 
      return ADC2_R0;
  }
  return 0;
}

bool HAL_adc_ready() {
  switch (HAL_adc_select) {
    case 0: return (ADC1_HS & ADC_HS_COCO0);
    case 1: return (ADC2_HS & ADC_HS_COCO0);
  }
  return false;
}

#endif // __IMXRT1062__
