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

#if defined(__IMXRT1062__)

#include "../../inc/MarlinConfig.h"

#if ENABLED(USE_WATCHDOG)

#if defined(WARN_UNTESTED)
#warning Teensy4.0 > Watchdog: This code has not been tested yet
#endif

#include "watchdog.h"

void watchdog_init() {
  WDOG1_WMCR = 0; // (PDE: power down enabled) must be done within 16 seconds of power up ?

  WDOG1_WCR = WDOG_WCR_WT(3) // set 2 second counter ?
            | WDOG_WCR_SRE;   // use new robust software reset ?
  
  WDOG1_WCR |= WDOG_WCR_WDE;
}

#endif // USE_WATCHDOG

#endif // __IMXRT1062__
