/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/
#include "leds.h"
#include "buttons.h"
#include "wheel.h"

#define BUTTON_X                                BUTTON_S2 
#define BUTTON_SQUARE                           BUTTON_S3
#define BUTTON_O                                BUTTON_S4
#define BUTTON_TRIANGLE                         BUTTON_S5
#define BUTTON_L1                               BUTTON_S6
#define BUTTON_R1                               BUTTON_S7
#define BUTTON_L2                               BUTTON_S8
#define BUTTON_R2                               BUTTON_S9
#define BUTTON_SELECT                           BUTTON_S10
#define BUTTON_START                            BUTTON_S11
#define BUTTON_LEFT_STICK                       BUTTON_S12
#define BUTTON_RIGHT_STICK                      BUTTON_S13
#define BUTTON_HOME                             BUTTON_S14
#define LED_USB_DEVICE_STATE                    LED_D1
#define WHEEL                                   WHEEL_W1
#define PEDAL_GAS                               PEDAL_GAS
#define BREAK_GAS                               BREAK_GAS

#define self_power                              1

