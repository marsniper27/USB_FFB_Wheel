/* 
 * File:   wheel.h
 * Author: kyle.marshall
 *
 * Created on May 16, 2018, 1:18 PM
 */

#include <stdbool.h>
#include <stdint.h>


#ifndef WHEEL_H
#define	WHEEL_H


#define POT                 0
#define ENCODER             1 //use for load cell for pedal as well

bool WHEEL_Type = POT; //set according to wheel type

bool BREAK_Type = POT; //set according to wheel type

bool CLUTCH_Type = POT; //set according to wheel type

/*** Wheel Definitions *********************************************/
typedef enum
{
    WHEEL_NONE,
    WHEEL_W1
            
} WHEEL;

typedef enum
{
    PEDAL_NONE,
    PEDAL_BREAK,
    PEDAL_CLUTCH
            
} PEDAL;

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
uint8_t WHEEL_Position(WHEEL wheel);

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
void WHEEL_Enable(WHEEL wheel);         // Enables the Wheel.

void WHEEL_Home(WHEEL wheel);           // Returns the wheel to the home position.

uint8_t WHEEL_Test(WHEEL wheel);        // Cycles through the full strange of the axis
uint8_t PEDAL_Position(PEDAL pedal);    // Returns the position of the provided pedal.

#endif	/* WHEEL_H */