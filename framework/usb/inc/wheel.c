/********************************************************************
 * File:   wheel.h
 * Author: kyle.marshall
 *
 * Created on May 16, 2018, 1:18 PM
 *******************************************************************/

#include <xc.h>
#include <stdint.h>
#include <wheel.h>
#include <pic18f45k50.h>

/*** Button Definitions *********************************************/
#define W1_PORT  PORTAbits.RA0

#define W1_TRIS  TRISAbits.TRISA0
//      S4       MCLR button

#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define PIN_DIGITAL         1
#define PIN_ANALOG          0

#define _XTAL_FREQ 20000000


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
uint8_t WHEEL_Position(WHEEL wheel)
{    
    switch(wheel)
    {
        case WHEEL_W1:
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
        case WHEEL_W1:
            W1_TRIS = PIN_INPUT;
            ADCON0bits.ADON = 1;
            ADCON0bits.CHS = 0b0000;
            ADCON1bits.NVCFG = 0;
            ADCON1bits.PVCFG = 0;
            
        case WHEEL_NONE:
            break;
    }
}