/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <buttons.h>

/*** Button Definitions *********************************************/
//      S1       MCLR reset
#define S2_PORT 	PORTBbits.RB7 //AN11
#define S3_PORT 	PORTBbits.RB6 
#define S4_PORT 	PORTBbits.RB5 
#define S5_PORT 	PORTBbits.RB4 
#define S6_PORT 	PORTBbits.RB3 
#define S7_PORT 	PORTBbits.RB2 
#define S8_PORT 	PORTBbits.RB1 
#define S9_PORT 	PORTBbits.RB0 
#define S10_PORT 	PORTDbits.RD7 
#define S11_PORT 	PORTDbits.RD6 
#define S12_PORT 	PORTDbits.RD5 
#define S13_PORT 	PORTDbits.RD4 
#define S14_PORT 	PORTDbits.RD3

#define S2_TRIS 	TRISBbits.TRISB7
#define S3_TRIS 	TRISBbits.TRISB6
#define S4_TRIS 	TRISBbits.TRISB5
#define S5_TRIS 	TRISBbits.TRISB4
#define S6_TRIS 	TRISBbits.TRISB3
#define S7_TRIS 	TRISBbits.TRISB2
#define S8_TRIS 	TRISBbits.TRISB1
#define S9_TRIS 	TRISBbits.TRISB0
#define S10_TRIS 	TRISDbits.TRISD7
#define S11_TRIS 	TRISDbits.TRISD6
#define S12_TRIS 	TRISDbits.TRISD5
#define S13_TRIS 	TRISDbits.TRISD4
#define S14_TRIS 	TRISDbits.TRISD3

#define S4_ANSEL    ANSELBbits.ANSB5
#define S5_ANSEL    ANSELBbits.ANSB4
#define S6_ANSEL 	ANSELBbits.ANSB3 
#define S7_ANSEL 	ANSELBbits.ANSB2 
#define S8_ANSEL 	ANSELBbits.ANSB1 
#define S9_ANSEL 	ANSELBbits.ANSB0
#define S10_ANSEL   ANSELDbits.ANSD7
#define S11_ANSEL   ANSELDbits.ANSD6
#define S12_ANSEL 	ANSELDbits.ANSD5 
#define S13_ANSEL 	ANSELDbits.ANSD4 
#define S14_ANSEL 	ANSELDbits.ANSD3 

#define BUTTON_PRESSED      0
#define BUTTON_NOT_PRESSED  1

#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define PIN_DIGITAL         0
#define PIN_ANALOG          1

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
bool BUTTON_IsPressed(BUTTON button)
{
    switch(button)
    {
        case BUTTON_S2:
            return ( (S2_PORT == BUTTON_PRESSED) ? true : false);
			
        case BUTTON_S3:
            return ( (S3_PORT == BUTTON_PRESSED) ? true : false);
            
        case BUTTON_S4:
            return ( (S4_PORT == BUTTON_PRESSED) ? true : false);
            
        case BUTTON_S5:
            return ( (S5_PORT == BUTTON_PRESSED) ? true : false);
        
        case BUTTON_S6:
            return ( (S6_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S7:
            return ( (S7_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S8:
            return ( (S8_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S9:
            return ( (S9_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S10:
            return ( (S10_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S11:
            return ( (S11_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S12:
            return ( (S12_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S13:
            return ( (S13_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_S14:
            return ( (S14_PORT == BUTTON_PRESSED) ? true : false);

        case BUTTON_NONE:
            return false;
    }
    
    return false;
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
void BUTTON_Enable(BUTTON button)
{
    switch(button)
    {
        case BUTTON_S2:
            S2_TRIS = PIN_INPUT;
            break;
			
        case BUTTON_S3:
            S3_TRIS = PIN_INPUT;
            break;
        
        case BUTTON_S4:
            S4_TRIS = PIN_INPUT;
            S4_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S5:
            S5_TRIS = PIN_INPUT;
            S5_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S6:
            S6_TRIS = PIN_INPUT;
            S6_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S7:
            S7_TRIS = PIN_INPUT;
            S7_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S8:
            S8_TRIS = PIN_INPUT;
            S8_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S9:
            S9_TRIS = PIN_INPUT;
            S9_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S10:
            S10_TRIS = PIN_INPUT;
            S10_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S11:
            S11_TRIS = PIN_INPUT;
            S11_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S12:
            S12_TRIS = PIN_INPUT;
            S12_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S13:
            S13_TRIS = PIN_INPUT;
            S13_ANSEL = PIN_DIGITAL;
            break;

        case BUTTON_S14:
            S14_TRIS = PIN_INPUT;
            S14_ANSEL = PIN_DIGITAL;
            break;
            
        case BUTTON_NONE:
            break;
    }
}
