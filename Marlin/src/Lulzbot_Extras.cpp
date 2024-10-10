/**********************
 * Lulzbot_Extras.cpp *
 **********************/

/****************************************************************************
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <http://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

#include "MarlinCore.h"
#include "module/planner.h"
#include "module/stepper.h"
#include "module/endstops.h"
#include "gcode/gcode.h"
#if ENABLED(TOUCH_UI_FTDI_EVE)
    #include "lcd/extui/ftdi_eve_touch_ui/ftdi_eve_lib/basic/ftdi_basic.h"
#endif
#include "Lulzbot_Extras.h"

/******************************** EMI MITIGATION *******************************/

#define LULZBOT_EMI_SHUTOFF(pin)             SET_OUTPUT(pin); WRITE(pin, LOW);

void LULZBOT_ON_STARTUP(void) {

    #if (MOTHERBOARD == BOARD_ARCHIM2)
        #if ENABLED(TAZ8)
            LULZBOT_EMI_SHUTOFF(GPIO_PB8_J20_11)
            LULZBOT_EMI_SHUTOFF(GPIO_PB7_J20_14)
            LULZBOT_EMI_SHUTOFF(GPIO_PA18_J20_21)
            LULZBOT_EMI_SHUTOFF(GPIO_PA17_J20_22)
        #else
            LULZBOT_EMI_SHUTOFF(GPIO_PB1_J20_5)
            LULZBOT_EMI_SHUTOFF(GPIO_PB0_J20_6)
            LULZBOT_EMI_SHUTOFF(GPIO_PB3_J20_7)
            LULZBOT_EMI_SHUTOFF(GPIO_PB2_J20_8)
            LULZBOT_EMI_SHUTOFF(GPIO_PB8_J20_11)
            LULZBOT_EMI_SHUTOFF(GPIO_PB4_J20_12)
            LULZBOT_EMI_SHUTOFF(GPIO_PB7_J20_14)
            LULZBOT_EMI_SHUTOFF(GPIO_PA18_J20_21)
            LULZBOT_EMI_SHUTOFF(GPIO_PA17_J20_22)
        #endif
    #endif
}

void LULZBOT_ON_REFLASH() {
    #if ENABLED(TOUCH_UI_FTDI_EVE)
        CLCD::set_brightness(0);
    #endif
}

/* Enable the probe pins only only when homing/probing,
 * as this helps reduce EMI by grounding the lines.
 *
 * On Mini:
 *   Z_MIN_PIN are the bed washers.
 *
 * On TAZ:
 *   Z_MIN_PIN corresponds to the Z-Home push button.
 *   Z_MIN_PROBE_PIN are the bed washers.
 */
#define LULZBOT_SET_PIN_STATE(pin, enable) \
    if(enable) { \
        /* Set as inputs with pull-up resistor */ \
        SET_INPUT(pin); \
        WRITE(pin, HIGH); \
        delay(5); /* The bed acts as a capacitor and takes a while to charge up */ \
    } else { \
        LULZBOT_EMI_SHUTOFF(pin); \
    }
#define LULZBOT_EXTRUDER_MOTOR_SHUTOFF_ON_PROBE(probing) \
        if(probing) { \
            planner.synchronize(); \
            stepper.disable_extruder(); \
        } else { \
            stepper.enable_extruder(); \
        }
