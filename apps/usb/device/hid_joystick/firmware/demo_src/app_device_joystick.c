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

#ifndef USBJOYSTICK_C
#define USBJOYSTICK_C

/** INCLUDES *******************************************************/
#include "usb.h"
#include "usb_device_hid.h"

#include "system.h"

#include "app_led_usb_status.h"

#include "stdint.h"

/** DECLARATIONS ***************************************************/
//http://www.microsoft.com/whdc/archive/hidgame.mspx
#define HAT_SWITCH_NORTH            0x0
#define HAT_SWITCH_NORTH_EAST       0x1
#define HAT_SWITCH_EAST             0x2
#define HAT_SWITCH_SOUTH_EAST       0x3
#define HAT_SWITCH_SOUTH            0x4
#define HAT_SWITCH_SOUTH_WEST       0x5
#define HAT_SWITCH_WEST             0x6
#define HAT_SWITCH_NORTH_WEST       0x7
#define HAT_SWITCH_NULL             0x8

/** TYPE DEFINITIONS ************************************************/
typedef union _INTPUT_CONTROLS_TYPEDEF
{
    struct
    {
        struct
        {
            uint8_t square:1;
            uint8_t x:1;
            uint8_t o:1;
            uint8_t triangle:1;
            uint8_t L1:1;
            uint8_t R1:1;
            uint8_t L2:1;
            uint8_t R2:1;//
            uint8_t select:1;
            uint8_t start:1;
            uint8_t left_stick:1;
            uint8_t right_stick:1;
            uint8_t home:1;
            uint8_t :3;    //filler
        } buttons;
        struct
        {
            uint8_t hat_switch:4;
            uint8_t :4;//filler
        } hat_switch;
        struct
        {
            uint8_t X;
            uint8_t Y;
            uint8_t Z;
            uint8_t Rz;
        } analog_stick;
    } members;
    uint8_t val[7];
} INPUT_CONTROLS;


/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#if defined(FIXED_ADDRESS_MEMORY)
    #if defined(COMPILER_MPLAB_C18)
        #pragma udata JOYSTICK_DATA=JOYSTICK_DATA_ADDRESS
            INPUT_CONTROLS joystick_input;
        #pragma udata
    #elif defined(__XC8)
        INPUT_CONTROLS joystick_input @ JOYSTICK_DATA_ADDRESS;
    #endif
#else
    INPUT_CONTROLS joystick_input;
#endif


USB_VOLATILE USB_HANDLE lastTransmission = 0;


bool Buttons_Pressed = 0; //our new bool to track if any button is pressed.
/*********************************************************************
* Function: void APP_DeviceJoystickInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceJoystickInitialize(void)
{  
    //initialize the variable holding the handle for the last
    // transmission
    lastTransmission = 0;

    //enable the HID endpoint
    USBEnableEndpoint(JOYSTICK_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
}//end UserInit

/*********************************************************************
* Function: void APP_DeviceJoystickTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceJoystickInitialize() and APP_DeviceJoystickStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceJoystickTasks(void)
{  
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        /* Jump back to the top of the while loop. */
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended() == true )
    {
        /* Jump back to the top of the while loop. */
        return;
    }

    //If the last transmission is complete
    if(!HIDTxHandleBusy(lastTransmission))
    {
        if(BUTTON_IsPressed(BUTTON_X) == true)
		{
			joystick_input.members.buttons.x = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.x = 0;
		}
		if(BUTTON_IsPressed(BUTTON_SQUARE) == true)
		{
			joystick_input.members.buttons.square = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.square = 0;
		}
		if(BUTTON_IsPressed(BUTTON_O) == true)
		{
			joystick_input.members.buttons.o = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.o = 0;
		}
		if(BUTTON_IsPressed(BUTTON_TRIANGLE) == true)
		{
			joystick_input.members.buttons.triangle = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.triangle = 0;
		}
		if(BUTTON_IsPressed(BUTTON_L1) == true)
		{
			joystick_input.members.buttons.L1 = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.L1 = 0;
		}
		if(BUTTON_IsPressed(BUTTON_R1) == true)
		{
			joystick_input.members.buttons.R1 = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.R1 = 0;
		}
		if(BUTTON_IsPressed(BUTTON_L2) == true)
		{
			joystick_input.members.buttons.L2 = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.L2 = 0;
		}
		if(BUTTON_IsPressed(BUTTON_R2) == true)
		{
			joystick_input.members.buttons.R2 = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.R2 = 0;
		}
		if(BUTTON_IsPressed(BUTTON_SELECT) == true)
		{
			joystick_input.members.buttons.select = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.select = 0;
		}
		if(BUTTON_IsPressed(BUTTON_START) == true)
		{
			joystick_input.members.buttons.start = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.start = 0;
		}
		if(BUTTON_IsPressed(BUTTON_LEFT_STICK) == true)
		{
			joystick_input.members.buttons.left_stick = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.left_stick = 0;
		}
		if(BUTTON_IsPressed(BUTTON_RIGHT_STICK) == true)
		{
			joystick_input.members.buttons.right_stick = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.right_stick = 0;
		}
		if(BUTTON_IsPressed(BUTTON_HOME) == true)
		{
			joystick_input.members.buttons.home = 1;
			Buttons_Pressed = 1;
		}
		else
		{
			joystick_input.members.buttons.home = 0;
		}

            //Move the hat switch to the "east" position
            joystick_input.members.hat_switch.hat_switch = HAT_SWITCH_EAST;

            //Move the X and Y coordinates to the their extreme values (0x80 is
            //  in the middle - no value).
            joystick_input.members.analog_stick.X = WHEEL_Position(WHEEL_W1);
            joystick_input.members.analog_stick.Y = 0;
            
        //if any button was pressed we will send a message with the currently pressed buttons.
		if(Buttons_Pressed)
		{
            //Send the packet over USB to the host.
            lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
        }
        if(!Buttons_Pressed)
        {
            //Reset values of the controller to default state

            //Buttons
            joystick_input.val[0] = 0x00;
            joystick_input.val[1] = 0x00;

            //Hat switch
            joystick_input.val[2] = 0x08;

            //Analog sticks
            joystick_input.val[3] = 0x80;
            joystick_input.val[4] = 0x80;
            joystick_input.val[5] = 0x80;
            joystick_input.val[6] = 0x80;

            //Send the 8 byte packet over USB to the host.
            lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
        }
    }
}//end ProcessIO

#endif
