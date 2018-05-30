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


////////////////////////////////////// FORCE FEEDBACK ///////////////////////////////////////////////

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

typedef union _EFFECT_BLOCK
{
 struct
 {
  uint8_t set_effect_report;
  uint8_t effect_block_index;
  uint8_t parameter_block_offset;
         
  uint8_t rom_flag;
 }effect_block_parameters;

uint8_t val[5];
}EFFECT_BLOCK;

typedef union _EFFECT
{
 struct
 {
  uint8_t effect_type;
  uint8_t effect_duration;
  uint8_t effect_sample_period;
  uint8_t effect_gain;
  uint8_t effect_trigger_button;
  uint8_t trigger_repeat_interval;
  uint8_t axes_enable;
  uint8_t direction_enable;
  uint8_t direction;
  uint8_t start_delay;

  uint8_t type_specific_block_handle_number;
  uint8_t type_specific_block_handle_1;// index to the type specific block1.
  uint8_t type_specific_block_handle_2;// index to the type specific block2.


 }effect_parameters;
uint8_t val[13];
}EFFECT; // 12 effects

typedef union _ENVELOPE_BLOCK
{
 struct
 {
  uint8_t attack_level;
  uint8_t attack_time;
  uint8_t fade_level;
  uint8_t fade_time;
 }envelope_block_parameters;

uint8_t val[4];
}ENVELOPE_BLOCK; // 12 effects, max 7 envelopes. 1 envelope per effect that support envelopes

typedef union _CONDITION_BLOCK
{
 struct
 {
  uint8_t cp_offset;
  uint8_t positive_coefficient;
  uint8_t negative_coefficient;
  uint8_t positive_saturation;
  uint8_t negative_saturation;
  uint8_t dead_band;
 }condition_block_parameters;

uint8_t val[6];
}CONDITION_BLOCK; // 12 effects, max 8 conditions. 4 per axis x/y. 2 conditions(x/y) per effect that support conditions

typedef union _PERIODIC_BLOCK
{
 struct
 {
  uint8_t offset;
  uint8_t magnitude;
  uint8_t phase;
  uint8_t period;
 }periodic_block_parameters;

uint8_t val[4];
}PERIODIC_BLOCK;// 12 effects, max 5 PERIODIC_BLOCKs. 1 PERIODIC_BLOCK per effect that support PERIODIC_BLOCK

typedef union _CONSTANT_FORCE_BLOCK
{
 struct
 {
  uint8_t magnitude;
 }constant_force_block_parameters;

uint8_t val[1];
}CONSTANT_FORCE_BLOCK; // only for constant force. idk, maybe change later.

typedef union _RAMP_FORCE_BLOCK
{
 struct
 {
  uint8_t ramp_start;
  uint8_t ramp_end;
 }ramp_force_block_parameters;

uint8_t val[2];
}RAMP_FORCE_BLOCK; // same a constant force, only 1 effect uses it, and it needs only 1 structure.

typedef union _EFFECT_OPERATIONS
{
 struct
 {
  uint8_t op_effect_start;
  uint8_t op_effect_start_solo;
  uint8_t op_effect_stop;
  uint8_t loop_count;
 }effect_operations_parameters;

uint8_t val[4];
}EFFECT_OPERATIONS;

typedef union _DEVICE_GAIN
{
 struct
 {
  uint8_t device_gain;
 }device_gain_parameters;

uint8_t val[1];
}DEVICE_GAIN;


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
        unsigned char ReceivedDataBuffer[64] @ HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
        unsigned char ToSendDataBuffer[64] @ HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;
        INPUT_CONTROLS joystick_input @ ToSendDataBuffer;
    #endif
#else
    INPUT_CONTROLS joystick_input;
#endif


volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;
//USB_VOLATILE USB_HANDLE lastTransmission = 0;
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
    //lastTransmission = 0; ////////// I commented

    //enable the HID endpoint
    //USBEnableEndpoint(JOYSTICK_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);////////// I commented
    
    //initialize the variable holding the handle for the last
    // transmission
    USBInHandle = 0;

    //enable the HID endpoint
    USBEnableEndpoint(JOYSTICK_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(JOYSTICK_EP,(uint8_t*)&ReceivedDataBuffer[0],64);
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

    if(!HIDRxHandleBusy(USBOutHandle))
    {   
        
        USBSetEffect();
        
        USBInHandle = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&ToSendDataBuffer[0],64);//send our response
        //We just received a packet of data from the USB host.
        //Check the first uint8_t of the packet to see what command the host
        //application software wants us to fulfill.
        switch(ReceivedDataBuffer[0])				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
            /*
            case COMMAND_TOGGLE_LED:  //Toggle LEDs command
                LED_Toggle(LED_USB_DEVICE_HID_CUSTOM);
                break;
            case COMMAND_GET_BUTTON_STATUS:  //Get push button state
                //Check to make sure the endpoint/buffer is free before we modify the contents
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    ToSendDataBuffer[0] = 0x81;				//Echo back to the host PC the command we are fulfilling in the first uint8_t.  In this case, the Get Pushbutton State command.
                    if(BUTTON_IsPressed(BUTTON_USB_DEVICE_HID_CUSTOM) == false)	//pushbutton not pressed, pull up resistor on circuit board is pulling the PORT pin high
                    {
                            ToSendDataBuffer[1] = 0x01;
                    }
                    else									//sw3 must be == 0, pushbutton is pressed and overpowering the pull up resistor
                    {
                            ToSendDataBuffer[1] = 0x00;
                    }
                    //Prepare the USB module to send the data packet to the host
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;

            case COMMAND_READ_POTENTIOMETER:	//Read POT command.  Uses ADC to measure an analog voltage on one of the ANxx I/O pins, and returns the result to the host
                {
                    uint16_t pot;

                    //Check to make sure the endpoint/buffer is free before we modify the contents
                    if(!HIDTxHandleBusy(USBInHandle))
                    {
                        //Use ADC to read the I/O pin voltage.  See the relevant HardwareProfile - xxxxx.h file for the I/O pin that it will measure.
                        //Some demo boards, like the PIC18F87J50 FS USB Plug-In Module board, do not have a potentiometer (when used stand alone).
                        //This function call will still measure the analog voltage on the I/O pin however.  To make the demo more interesting, it
                        //is suggested that an external adjustable analog voltage should be applied to this pin.

                        pot = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);

                        ToSendDataBuffer[0] = 0x37;  	//Echo back to the host the command we are fulfilling in the first uint8_t.  In this case, the Read POT (analog voltage) command.
                        ToSendDataBuffer[1] = (uint8_t)pot; //LSB
                        ToSendDataBuffer[2] = pot >> 8;     //MSB


                        //Prepare the USB module to send the data packet to the host
                        USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ToSendDataBuffer[0],64);
                    }
                }
                break;
             * */
        } //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(JOYSTICK_EP, (uint8_t*)&ReceivedDataBuffer[0], 64);
    }
    
    //If the last transmission is complete
    if(!HIDTxHandleBusy(USBInHandle))
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
            USBInHandle = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
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
            USBInHandle = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
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
    set_get_effect_structure.SET_REPORT_REQUEST.report_id=ReceivedDataBuffer[0]; // 1: based on the pid report descriptor
    set_get_effect_structure.SET_REPORT_REQUEST.effect_type=ReceivedDataBuffer[1];// the type of effect, based on pid report descriptor
    set_get_effect_structure.SET_REPORT_REQUEST.byte_count=ReceivedDataBuffer[2]; // this device does not support custom effects

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
void USBSendPIDBlockLoadReport(void)
{
    /*
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
    */

     USBEP0SendRAMPtr((uint8_t*)&set_get_effect_structure.PID_BLOCK_LOAD_REPORT, 5, USB_EP0_RAM); 
}

/*
basically, look into the received data, and save things...in the effect block indexes, etc etc
*/
void USBSetDataEffect(void)
{
 // for now i should have tricked the host to belive the report is accepted, and to send me the data
// the data indeed is sent, and saved, but i lied, and i'm doing nothing with that data, censored the host
// and censored PID book from usb.org. , i had to read the entire direct input, line by line,
// to make sense of that book. first is apples, then oranges, but in the end 
// apples and oranges are the same thing. sometimes they call it (effect) apples
// but they mean (brown)apples, and effect has nothing to do with it. and it's not brown, it's red, but 
// is encoded as RGB(00FF00), then they ask for the alpha channel. but you are stucked with something green
// it's just green, no mass, no shape, no nothing, worse than a void pointer.
// basically you have a green void pointer to a brown index of apples.
// but they let you guess that ! tricky :D 
// no, seriously, I'm out.!

// here we should read the incoming data on hid_report_out, and save the values in the relative //effect that is created, i am not doing it for now, because this is just a test version of the firmware
}

void USER_SET_REPORT_HANDLER(void)
 {
    //I get the set report, then i get the DATA !
    // THE DEVICE NOW NEEDS TO ALLOCATE THE EFFECT
    // ONCE THE EFFECT IS ALLOCATED; THE HOST SENDS A GET REPORT, function above
    /*
    Offset Field Size Value Description
    0 bmRequestType 1 10100001b From device to host
    1 bRequest 1 0x01 Get_Report
    2 wValue 2 0x0203 Report ID (2) and Report Type (feature)
    4 wIndex 2 0x0000 Interface
    6 wLength 2 0x0500 Number of bytes to transfer in the data phase
    (5 bytes)
    */
    // DEVICE RESPONDS TO THE GET REPORT; WITH THE PID BLOCK LOAD REPORT

    //USBSetEffect();
    // I HAVE TO: 1: BE SURE I CHECK THE FEATURE, BECAUSE: REASONS

    switch(SetupPkt.W_Value.byte.LB) 
    {
        case 0x01: // report ID: 1, SET NEW EFFECT,APPLICATION DATA, etc , and now ?
           // NOW I CHECK THE REPORT TYPE ! INPUT? OUTPUT? FEATURE ? 

            if(SetupPkt.W_Value.byte.HB==0x02) // FEATURE, the host wants to send me the data, to set the effects. aka effect start 
            {
                // READ THE DATA, FEEL THE DATA, SAVE THE DATA! and set the data effect, read it, and set it
                USBEP0Receive((uint8_t*)&ReceivedDataBuffer[0], SetupPkt.wLength, USBSetDataEffect);
            } 

            if(SetupPkt.W_Value.byte.HB==0x03) 
            {
                // READ THE DATA, FEEL THE DATA, SAVE THE DATA! and set the effect
                USBEP0Receive((uint8_t*)&ReceivedDataBuffer[0], SetupPkt.wLength, USBSetEffect); 
            } 
            break;
    }
 }
#endif
