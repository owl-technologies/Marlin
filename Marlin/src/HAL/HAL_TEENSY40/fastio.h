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
#pragma once

#if defined(WARN_UNTESTED)
#warning Teensy4.0 > FastIO: This code has not been tested yet
#endif
/**
 * Fast I/O Routines for Teensy 3.5 and Teensy 3.6
 * Use direct port manipulation to save scads of processor time.
 * Contributed by Triffid_Hunter and modified by Kliment, thinkyhead, Bob-the-Kuhn, et.al.
 */

#ifndef MASK
  #define MASK(PIN) (1 << PIN)
#endif


/**
 * Magic I/O routines
 *
 * Now you can simply SET_OUTPUT(PIN); WRITE(PIN, HIGH); WRITE(PIN, LOW);
 *
 * Why double up on these macros? see http://gcc.gnu.org/onlinedocs/cpp/Stringification.html
 */
#define IOMUX_ALT(n) (n)
#define IOMUX_SION (1<<4)
#define IOMUX_GPIO IOMUX_ALT(5)
#define IOMUX_DRIVE_STRENGTH(n) (((n)&0x7)<<3)

#define _READ(P) bool(CORE_PIN ## P ## _PINREG & CORE_PIN ## P ## _BITMASK)

#define _WRITE(P,V) do{ \
  if (V) CORE_PIN ## P ## _PORTSET = CORE_PIN ## P ## _BITMASK; \
  else CORE_PIN ## P ## _PORTCLEAR = CORE_PIN ## P ## _BITMASK; \
}while(0)

#define _TOGGLE(P) (*(&(CORE_PIN ## P ## _PORTCLEAR)+1) = CORE_PIN ## P ## _BITMASK)

#define _SET_INPUT(P) do{ \
  CORE_PIN ## P ## _CONFIG = IOMUX_GPIO | IOMUX_SION; \
  CORE_PIN ## P ## _PADCONFIG = IOMUXC_PAD_HYS | IOMUXC_PAD_PUE; \
  CORE_PIN ## P ## _DDRREG &= ~(CORE_PIN ## P ## _BITMASK); \
}while(0)

#define _SET_OUTPUT(P) do{ \
  CORE_PIN ## P ## _CONFIG = IOMUX_GPIO; \
  CORE_PIN ## P ## _PADCONFIG = IOMUX_DRIVE_STRENGTH(1); \
  CORE_PIN ## P ## _DDRREG |= CORE_PIN ## P ## _BITMASK; \
}while(0)

#define _SET_INPUT_PULLUP(P) do{ \
  CORE_PIN ## P ## _CONFIG = IOMUX_GPIO | IOMUX_SION; \
  CORE_PIN ## P ## _PADCONFIG = IOMUXC_PAD_HYS | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(1); \
  CORE_PIN ## P ## _DDRREG &= ~(CORE_PIN ## P ## _BITMASK); \
}while(0)

#define _IS_INPUT(P)    ((CORE_PIN ## P ## _DDRREG & CORE_PIN ## P ## _BITMASK) == 0)
#define _IS_OUTPUT(P)   ((CORE_PIN ## P ## _DDRREG & CORE_PIN ## P ## _BITMASK) != 0)

#define READ(IO)              _READ(IO)

#define WRITE(IO,V)           _WRITE(IO,V)
#define TOGGLE(IO)            _TOGGLE(IO)

#define SET_INPUT(IO)         _SET_INPUT(IO)
#define SET_INPUT_PULLUP(IO)  _SET_INPUT_PULLUP(IO)
#define SET_OUTPUT(IO)        _SET_OUTPUT(IO)
#define SET_PWM(IO)            SET_OUTPUT(IO)

#define IS_INPUT(IO)          _IS_INPUT(IO)
#define IS_OUTPUT(IO)         _IS_OUTPUT(IO)

#define OUT_WRITE(IO,V)       do{ SET_OUTPUT(IO); WRITE(IO,V); }while(0)

// digitalRead/Write wrappers
#define extDigitalRead(IO)    digitalRead(IO)
#define extDigitalWrite(IO,V) digitalWrite(IO,V)

#define PWM_PIN(P)            digitalPinHasPWM(P)

/**
 * Ports, functions, and pins
 */

#define DIO0_PIN 8