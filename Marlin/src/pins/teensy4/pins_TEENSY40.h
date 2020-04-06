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

/****************************************************************************************
* Teensy 4.0 (IMXRT1062) Breadboard pin assignments as used by me.
* http://www.pjrc.com/teensy/teensyduino.html
****************************************************************************************/

#if !IS_TEENSY40
  #error "Oops! Select 'Teensy 4.0' in 'Tools > Board.'"
#else
  #define BOARD_INFO_NAME "Teensy40"
#endif

// NOTE: Q: what does it do, is this needed ?
#define AT90USB 1286   // Disable MarlinSerial etc.
#define USBCON //1286  // Disable MarlinSerial etc.
/*

  Limitz plan for Teensy40:
  (no part cooling fan, I use only PC-ABS)
  (X axis trinamic sensorless homing)
  (Y axis trinamic sensorless homing)
  (E motor controller is simple 8825, optionally 2 in copy or mirror configuration in which case 2 heaters are needed)
  (Optional second Y axis motor which takes same input as initial y motor except trinamic SPI)
  (Z homing is done with BLTOUCH)
  (Second servo optional for automatic calibration with another type of probe)
  (SPI for the trinamic motors is the same as SPI for the LCD)
  (Trinamic functionality built around the 2130, so the DIAG pins can be reused when SPI_ENDSTOPS)


                                                      USB*
                                                     #####
                                        GND |--------#####--------| VIN 5V
      X_STEP_PIN    ==  CRX2    RX1  PWM  0 |                     | Analog GND
      X_DIR_PIN     ==  CTX2    TX1  PWM  1 | |        SDIO*    | | 3.3V
      Y_STEP_PIN    ==               PWM  2 | | | | | | | | | | | | 23  A9 PWM        CRX1  MCLK1   == E0_STEP_PIN
      Y_DIR_PIN     ==  SCL2         PWM  3 |   | | | | | | | |   | 22  A8 PWM        CTX1          == E0_DIR_PIN
      Z_STEP_PIN    ==  SDA2         PWM  4 |   3 3 G 3 3 3 3 3   | 21  A7      RX5         BCLK1   == E0_ENABLE_PIN
      Z_DIR_PIN     ==  MISO1        PWM  5 |   4 5 N 6 V 7 8 9   | 20  A6      TX5         LRCLK1  == TEMP_BED_PIN 
      X_ENABLE_PIN  ==               PWM  6 |       D   3         | 19  A5 PWM        SCL0          == HEATER_BED
      Y_ENABLE_PIN  ==          RX2  PWM  7 |                     | 18  A4 PWM        SDA0          == TMC_CS_X
      Z_ENABLE_PIN  ==          TX2  PWM  8 | 25* 27* 29* 31* 33* | 17  A3      TX4   SDA1          == TEMP_1_PIN
      TMC_CS_Y1     ==               PWM  9 |                     | 16  A2      RX4   SCL1          == TEMP_0_PIN
      TMC_CS_Y2     ==  CS           PWM 10 | 24* 26* 28* 30* 32* | 15  A1 PWM  RX2         SPDIFI  == HEATER_1
      SPI_MOSI      ==  MOSI         PWM 11 |                     | 14  A0 PWM  TX3         SPDIFO  == HEATER_2
      SPI_MISO      ==  MISO         PWM 12 |--*---*---*---*---*--| 13 LED PWM  CRX1  SCK           == SPI_SCK
                                               |   |   |   |   |
                                               V   3   G   P   O
                                               B   .   N   r   n
                                               a   3   D   o   /
                                               t   V       g   O
                                               t           r   f
                                                           a   f
                                                           m
                * [BOTTOM PINS]
                
      SERVO0_PIN    == SCL2  TX6  PWM  A10  <- [24] [25] -> A11  PWM  RX6  SDA2   == SERVO1_PIN 
      CONTR_FAN     == MOSI1           A12  <- [26] [27] -> A13            SCK1   == Z_STOP_PIN
      PARTS_FAN     ==       RX7  PWM       <- [28] [29] ->      PWM  TX7         == BTN_ENC
      PS_ON         ==       CRX3           <- [30] [31] ->           CTX3        == BTN_EN1
      NEOPIXELS     ==       OUT1B          <- [32] [33] ->      PWM       MCLK2  == BTN_EN2


                * [SDIO]
                
                  [34] -> DAT1  PWM  MISO2  == LCD_D4
                  [35] -> DAT2  PWM  MOSI2  == LCD_D5
                  [36] -> CLK   PWM  CS2    == LCD_D6
                  [37] -> CMD   PWM  SCK2   == LCD_D7
                  [38] -> DAT3  PWM  RX5    == LCD_PINS_RS
                  [39] -> DAT2  PWM  TX5    == LCD_PINS_ENABLE

                * [USB]
                  
                  [D-] [ D+] == HOST CONTROLLER MIDI PERIPHERALS (sliders, buttons etc)
*/

//
// Limit Switches
//
#define X_STOP_PIN         -1
#define Y_STOP_PIN         -1
#define Z_STOP_PIN         27
//
// Steppers
//
#define X_STEP_PIN         0
#define X_DIR_PIN          1
#define X_ENABLE_PIN       6
#define X_CS_PIN           18

#define Y_STEP_PIN         2
#define Y_DIR_PIN          3
#define Y_ENABLE_PIN       7
#define Y_CS_PIN         10
#define Y2_CS_PIN         11

#define Z_STEP_PIN         4
#define Z_DIR_PIN          5
#define Z_ENABLE_PIN       8

#define E0_STEP_PIN        23
#define E0_DIR_PIN         22
#define E0_ENABLE_PIN      21

#define HEATER_0_PIN       15
#define HEATER_1_PIN       14
#define HEATER_BED_PIN     19

#ifndef FAN_PIN
#define FAN_PIN            28 
#endif

#ifndef CONTROLLER_FAN_PIN
#define CONTROLLER_FAN_PIN 26 
#endif

#define TEMP_0_PIN          16   // Extruder / Analog pin numbering: 2 => A2
#define TEMP_1_PIN          17
#define TEMP_BED_PIN        20   // Bed / Analog pin numbering

#define SDSS               -1   // 8
#define LED_PIN            -1
#define PS_ON_PIN          30
#define ALARM_PIN          -1

#define FILWIDTH_PIN       -1
#define SERVO0_PIN         24
#define SERVO1_PIN         25

#ifndef NEOPIXEL_PIN
#define NEOPIXEL_PIN       32
#endif

#define TMC_USE_SW_SPI
#define TMC_SW_MOSI        34 // ?
#define TMC_SW_MISO        35 // ?
#define TMC_SW_SCK         36 // ?

#if HAS_SPI_LCD
  #define LCD_PINS_RS      38
  #define LCD_PINS_ENABLE  39
  #define LCD_PINS_D4      34
  #define LCD_PINS_D5      35
  #define LCD_PINS_D6      36
  #define LCD_PINS_D7      37
#endif

#if ENABLED(NEWPANEL)
  #define BTN_EN1          31
  #define BTN_EN2          33
  #define BTN_ENC          29
#endif

#if ENABLED(REPRAPWORLD_KEYPAD)
  #define SHIFT_OUT        31
  #define SHIFT_CLK        33
  #define SHIFT_LD         29
#endif
