/********************************************************************
 * File:   wheel.h
 * Author: kyle.marshall
 *
 * Created on May 16, 2018, 1:18 PM
 *******************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <buttons.h>

/*** Button Definitions *********************************************/
#define Wheel_PORT  PORTAbits.RA0

#define Wheel_TRIS  TRISAbits.TRISA0
//      S4       MCLR button

#define BUTTON_PRESSED      0
#define BUTTON_NOT_PRESSED  1

#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define PIN_DIGITAL         1
#define PIN_ANALOG          0

/*********************************************************************
* Function: bool BUTTON_IsPressed(BUTTON button);
*
* Overview: Returns the current state of the requested button
*
* PreCondition: button configured via BUTTON_SetConfiguration()
*
* Input: BUTTON button - enumeration of the buttons available in
*        this demo.  They should be meaningful names and not the names 
*        of the buttons on the silkscreen on the board (as the demo 
*        code may be ported to other boards).
*         i.e. - ButtonIsPressed(BUTTON_SEND_MESSAGE);
*
* Output: TRUE if pressed; FALSE if not pressed.
*
********************************************************************/
unsigned int WHEEL_Turned(WHEEL wheel)
{
    ADC_Init();
    
    switch(wheel)
    {
        case WHEEL_1:
            ADCON0 &= 0xC5;              //Clearing channel selection bits
            ADCON0 |= 0<<3;        //Setting channel selection bits
            __delay_ms(2);               //Acquisition time to charge hold capacitor
            GO_nDONE = 1;                //Initializes A/D conversion
            while(GO_nDONE);             //Waiting for conversion to complete
            return ((ADRESH<<8)+ADRESL); //Return result
            
        case WHEEL_NONE:
            return 128;
    }
    
    return 128;
}

/*********************************************************************
* Function: void BUTTON_Enable(BUTTON button);
*
* Overview: Returns the current state of the requested button
*
* PreCondition: button configured via BUTTON_SetConfiguration()
*
* Input: BUTTON button - enumeration of the buttons available in
*        this demo.  They should be meaningful names and not the names
*        of the buttons on the silkscreen on the board (as the demo
*        code may be ported to other boards).
*         i.e. - ButtonIsPressed(BUTTON_SEND_MESSAGE);
*
* Output: None
*
********************************************************************/
void WHEEL_Enable(WHEEL wheel)
{
    switch(wheel)
    {
        case WHEEL_1:
            Wheel_TRIS = PIN_INPUT;
            ANCON1bits.PCFG0 = PIN_ANALOG;
            
        case WHEEL_NONE:
            break;
    }
}

void ADC_Init()
{
  ADCON0 = 0x81;               //Turn ON ADC and Clock Selection
  ADCON1 = 0x0E;               //All pins as Analog Input and setting Reference Voltages
}