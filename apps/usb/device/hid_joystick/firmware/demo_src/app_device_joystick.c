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
            uint8_t RID;
        }REPORT_ID;
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
            uint16_t X;
            uint16_t Y;
            uint16_t Z;
            uint16_t Rz;
        } analog_stick;
    } members;
    uint8_t val[12];
} INPUT_CONTROLS;

typedef union _SET_GET_EFFECT_STRUCTURE
{
    struct
    {
        uint8_t report_id;
        uint8_t effect_type;
        uint8_t byte_count; // valid only for custom force data effect.
          // custom force effects are not supported by this device.
    }SET_REPORT_REQUEST;
    struct
    {
        uint8_t report_id; // 2
        uint8_t effect_block_index; // index dell'effetto
        uint8_t block_load_status; // 1 ok, 2 -out of memory, 3 JC was here, or maybe not ? case: undefined.
        int ram_pool_available;
    }PID_BLOCK_LOAD_REPORT;

   uint8_t val[8];
}SET_GET_EFFECT_STRUCTURE;

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
SET_GET_EFFECT_STRUCTURE set_get_effect_structure;
uint8_t CONFIGURED_EFFECT_NUMBER[12];
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
            //joystick_input.members.analog_stick.X = WHEEL_Position(WHEEL_W1);
            joystick_input.members.analog_stick.X = WHEEL_Test(WHEEL_W1);
            joystick_input.members.analog_stick.Y = PEDAL_Position(PEDAL_BREAK);
            joystick_input.members.analog_stick.Z = PEDAL_Position(PEDAL_CLUTCH);
            
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
            
            //Add current wheel position as we all ways want this reported.
            //joystick_input.members.analog_stick.X = WHEEL_Position(WHEEL_W1);
            joystick_input.members.analog_stick.X = WHEEL_Test(WHEEL_W1);
            

            //Send the 8 byte packet over USB to the host.
            lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
        }
    }
}//end ProcessIO

/*******************************************************************
 * Function: void USBSetEffect(void)
 * 
 *
 * PreCondition: None
 *
 * Input: None
 * 
 * 
 *
 * Output: None
 *
 * Side Effects: None
 *
 * Overview: This function is called from USER_SET_REPORT_HANDLER
 * to handle the PID class specific request: SET REPORT REQUEST
 * SET REPORT REQUEST is issued by the host application when it needs
 * to create a new effect. The output report, specifies wich
 * effect type to create.
 * The firmware creates the effect ( sets CONFIGURED_EFFECT_NUMBER[EFFECT TYPE] to 1 )
 * and prepares the get_report for the host.
 * The set report request is saved in hid_report_out[n]
 * 
 *
 * Note: None
 *******************************************************************/

void USBSetEffect(void)
{
    set_get_effect_structure.SET_REPORT_REQUEST.report_id=hid_report_out[0]; // 1: based on the pid report descriptor
    set_get_effect_structure.SET_REPORT_REQUEST.effect_type=hid_report_out[1];// the type of effect, based on pid report descriptor
    set_get_effect_structure.SET_REPORT_REQUEST.byte_count=hid_report_out[2]; // this device does not support custom effects

    /*
     struct
     {
      BYTE report_id; // 2
      BYTE effect_block_index; // index dell'effetto
      BYTE block_load_status; // 1 ok, 2 -out of memory, 3 JC was here, or maybe not ? case: undefined.
      int ram_pool_available;
     }PID_BLOCK_LOAD_REPORT;

    BYTE val[8];
    }SET_GET_EFFECT_STRUCTURE;
    */
    switch(set_get_effect_structure.SET_REPORT_REQUEST.effect_type)
    { 
        case 1:// Usage ET Constant Force 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=1; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[0]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.

            break;
        case 2:// Usage ET Ramp 1
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=2; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[1]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 3:// Usage ET Square 2
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=3; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[2]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 4:// Usage ET Sine 3
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=4; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated.

            CONFIGURED_EFFECT_NUMBER[3]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 5:// Usage ET Triangle 4
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=5; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[4]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 6:// Usage ET Sawtooth Up 5
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=6; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[5]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 7:// Usage ET Sawtooth Down 6
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=7; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this, because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[6]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 8:// Usage ET Spring 7
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=8; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this  because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated.

            CONFIGURED_EFFECT_NUMBER[7]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 9:// Usage ET Damper 8
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=9; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this , because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[8]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 10:// Usage ET Inertia 9
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=10; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this , because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[9]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 11:// Usage ET Friction 10
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=11; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=1; // ok, i can load this , because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0xFFFF; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[10]=1; // I HAVE CONFIGURED CONSTANT FORCE EFFECT FOR THIS DEVICE
            // now, when the host issues a get_report blah blah, i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
        case 12:// Usage ET Custom Force Data 11 ! NOT SUPPORTED BY THIS DEVICE !
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.report_id=2; // 2= PID BLOCK LOAD REPORT
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.effect_block_index=0; //0=i can't create effect, 1 = CONSTANT FORCE, index in the array = effect_block_index-1 : 0
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.block_load_status=3; // ok, i can load this , because i have already memory for it, and because : reasons
            set_get_effect_structure.PID_BLOCK_LOAD_REPORT.ram_pool_available=0x0000; // i have no ideea why i need this, however i have no more memory except for the effects already preallocated. 

            CONFIGURED_EFFECT_NUMBER[11]=0; // 
            // now, when the host issues a get_report , i send the SET_GET_EFFECT_STRUCTURE.PID_BLOCK_LOAD_REPORT.
            break;
    } 
}

#endif
