/********************************************************************
 * File:   wheel.h
 * Author: kyle.marshall
 *
 * Created on May 16, 2018, 1:18 PM
 *******************************************************************/

#include <xc.h>
#include <wheel.h>
#include <pic18f45k50.h>


/*** Button Definitions *********************************************/
#define WA_PORT         PORTAbits.RA0
#define WB_PORT         PORTAbits.RA1
#define WZ_PORT         PORTAbits.RA2
#define GAS_PORT        PORTAbits.RA3
#define BREAK_PORT      PORTAbits.RA4


#define WA_TRIS         TRISAbits.TRISA0
#define WB_TRIS         TRISAbits.TRISA1
#define WZ_TRIS         TRISAbits.TRISA2
#define GAS_TRIS        TRISAbits.TRISA3
#define BREAK_TRIS      TRISAbits.TRISA4

#define WA_ANSEL        ANSELAbits.ANSA0
#define WB_ANSEL        ANSELAbits.ANSA1
#define WZ_ANSEL        ANSELAbits.ANSA2
#define GAS_ANSEL       ANSELAbits.ANSA3
#define BREAK_ANSEL     ANSELAbits.ANSA5


#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define PIN_DIGITAL         1
#define PIN_ANALOG          0

#define Home                0

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
uint16_t WHEEL_Position(WHEEL wheel)
{    
    switch(wheel)
    {
        case WHEEL_W1:
            if (WHEEL_Type == POT)
            {
               uint16_t digital;
               
                ADRESH=0;		/* Flush ADC output Register */
                ADRESL=0;   

                /* Channel 0 is selected i.e.(CHS3CHS2CHS1CHS0=0000) & ADC is disabled */
                ADCON0 =(ADCON0 & 0b11000011)|((0<<2) & 0b00111100); 

                ADCON0 |= ((1<<ADON)|(1<<GO));	/*Enable ADC and start conversion*/

                /* Wait for End of conversion i.e. Go/done'=0 conversion completed */
                while(ADCON0bits.GO_nDONE==1);

                //((ADRESH)<<8)|(ADRESL)
                digital = (ADRESH*256) | (ADRESL);	/*Combine 8-bit LSB and 2-bit MSB*/
                //digital = digital*4;  //unfortunately we currently are using only 8bits for a position so we must divide or ADC value to fit within out limit.
                return(digital);

            }
            else if (WHEEL_Type == ENCODER)
            {
                
            }
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
            WA_TRIS = PIN_INPUT;
            if (WHEEL_Type == POT)
            {
                 WA_ANSEL = PIN_ANALOG;
                /*
                ADCON1bits.CHSN3 = 0b1;
                ADCON1bits.NVCFG = 0;
                ADCON1bits.PVCFG = 0;
                ADCON0bits.CHS = 0b0000;                        
                ADCON0bits.ADON = 1;
                 */
                ADCON1 = 0x0E;	/* Ref vtg is VDD and Configure pin as analog pin */
                ADCON2 = 0x92;	/* Right Justified, 4Tad and Fosc/32. */
                ADRESH=0;		/* Flush ADC output Register */
                ADRESL=0;   
            }
            else if(WHEEL_Type == ENCODER)
            {
                WB_TRIS = PIN_INPUT;
                WZ_TRIS = PIN_INPUT;
                        
                WA_ANSEL = PIN_DIGITAL;
                WB_ANSEL = PIN_DIGITAL;
                WZ_ANSEL = PIN_DIGITAL;
            }
        case WHEEL_NONE:
            break;
    }
}

void PEDAL_Enable(PEDAL pedal)
{
    switch(pedal)
    {
        case PEDAL_GAS:
            GAS_TRIS = PIN_INPUT;
            if (GAS_Type == POT)
            {
                GAS_ANSEL = PIN_ANALOG;
               
            }
            else if(GAS_Type == LOADCELL)
            {
                
            }
            break;
        case PEDAL_BREAK:
            BREAK_TRIS = PIN_INPUT;
            if (BREAK_Type == POT)
            {
                BREAK_ANSEL = PIN_ANALOG;
            }
            else if(BREAK_Type == LOADCELL)
            {
                WB_TRIS = PIN_INPUT;
                WZ_TRIS = PIN_INPUT;
                        
                WA_ANSEL = PIN_DIGITAL;
                WB_ANSEL = PIN_DIGITAL;
                WZ_ANSEL = PIN_DIGITAL;
            }
        case PEDAL_NONE:
            break;
    }
}

void WHEEL_Home(WHEEL wheel)
{
    switch(wheel)
    {
        case WHEEL_W1:
            while(WZ_PORT != Home)
            {
                //we will use this to turn the motor to return the wheel to the home position when the device is activated.
                WZ_PORT=Home;
            }
            break;
        case WHEEL_NONE:
            break;
    }
}

uint16_t WHEEL_Test(WHEEL wheel)
{
    static uint16_t count = 0;
    static bool direction = 1;
    
    switch(wheel)
    {
        case WHEEL_W1:
            if(direction)
            {
                if(count < 255)
                {
                    count++;

                }
                if(count == 255)
                {
                    direction = 0;
                }
            }
            else if(!direction)
            {
                if(count > 0)
                {
                    count--;

                }
                if(count == 0)
                {
                    direction = 1;
                }
            }
            break;
            
        case WHEEL_NONE:
            break;
    }
    return count;
}


uint16_t PEDAL_Position(PEDAL pedal)
{
    switch(pedal)
    {
        case PEDAL_GAS:
            if (GAS_Type == POT)
            {
                /*ADCON0bits.CHS = 0011;       //Setting channel selection bits
                __delay_ms(2);               //Acquisition time to charge hold capacitor
                GO_nDONE = 1;                //Initializes A/D conversion
                while(GO_nDONE);             //Waiting for conversion to complete
                return ((ADRESH<<8)+ADRESL); //Return result
                 */
               //uint16_t digital;
                
                ADRESH=0;		/* Flush ADC output Register */
                ADRESL=0;   

                /* Channel 3 is selected i.e.(CHS3CHS2CHS1CHS0=0000) & ADC is disabled */
                //ADCON0 =(ADCON0 & 0b11001111)|((0<<2) & 0b00111100); 
                
                ADCON0bits.CHS = 0b0011;
                ADCON0bits.GO - 0;

                ADCON0 |= ((1<<ADON)|(1<<GO));	/*Enable ADC and start conversion*/

                /* Wait for End of conversion i.e. Go/done'=0 conversion completed */
                while(ADCON0bits.GO_nDONE==1);
                
                return((ADRESH*256) | (ADRESL));
            }
            else if (GAS_Type == ENCODER)
            {
                
            }
            break;
        case PEDAL_BREAK:
            if (BREAK_Type == POT)
            {
                /*ADCON0bits.CHS = 0011;       //Setting channel selection bits
                __delay_ms(2);               //Acquisition time to charge hold capacitor
                GO_nDONE = 1;                //Initializes A/D conversion
                while(GO_nDONE);             //Waiting for conversion to complete
                return ((ADRESH<<8)+ADRESL); //Return result
                 */
               //uint16_t digital;
                
                ADRESH=0;		/* Flush ADC output Register */
                ADRESL=0;   

                /* Channel 3 is selected i.e.(CHS3CHS2CHS1CHS0=0000) & ADC is disabled */
                //ADCON0 =(ADCON0 & 0b11001111)|((0<<2) & 0b00111100); 
                
                ADCON0bits.CHS = 0b0100;
                ADCON0bits.GO - 0;

                ADCON0 |= ((1<<ADON)|(1<<GO));	/*Enable ADC and start conversion*/

                /* Wait for End of conversion i.e. Go/done'=0 conversion completed */
                while(ADCON0bits.GO_nDONE==1);
                
                return((ADRESH*256) | (ADRESL));
            }
            else if (BREAK_Type == ENCODER)
            {
                
            }
            break;
            
        /*case PEDAL_CLUTCH:
            if (CLUTCH_Type == POT)
            {
                ADCON0bits.CHS = 0b0100;       //Setting channel selection bits
                __delay_ms(2);               //Acquisition time to charge hold capacitor
                GO_nDONE = 1;                //Initializes A/D conversion
                while(GO_nDONE);             //Waiting for conversion to complete
                return ((ADRESH<<8)+ADRESL); //Return result
            }
            else if (CLUTCH_Type == ENCODER)
            {
                
            }
            break;
        
         */
        case PEDAL_NONE:
            return 128;
    }
    
    return 128;
}