/*******************************************************************************
* File: main.c
*
* Version: 2.0
*
* Description:
*  This is the source code for the Power Management API example, which is
*  part of the cy_boot component. This project demonstrates the Hibernate low
*  power mode for PSoC 4 devices.
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/
#include <project.h>
#include <cyPm.h>
​
#define LIGHT_OFF                       (0u)
#define LIGHT_ON                        (1u)
​
/* Interrupt prototypes */
CY_ISR_PROTO(WDTIsrHandler);
​
#define SLEEP_INTERVAL_2S        2000                       /* millisecond */
#define ILO_FREQ                    32000                       /* Hz */
#define LOG_ROW_INDEX               (CY_FLASH_NUMBER_ROWS - 1)  /* last row */
​
/******************************************************************************
 * Global variables definition
 * ----------------------------------------------------------------------------
 * These varialbes should be populated to other modules. Header file contain 
 * the extern statement for these variables.
 ******************************************************************************/         
void InitWatchdog(uint16 sleep_interval);
​
/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function performs the following functions:
*   1. Enable global interrupts
*   2. Turns LED on
*   3. Makes a delay of 1000 ms is made for LED visibility.
*   5. Turns off LED to indicate the hibernate mode.
*   6. Switches to the hibernate Mode.
*   7. Wakes up from the hibernate mode with the SW button.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
​
​
int main()
{
    
    /*==========================================================================================
     * this code piece initializes the watchdog function 
     *==========================================================================================*/
    /* initialize watchdog */
    InitWatchdog(SLEEP_INTERVAL_2S);
    /* connect ISR routine to Watchdog interrupt */
    isr_WDT_StartEx(WDTIsrHandler); // The "isr_WDT" prefix comes from our TopDesign
    /* set the highest priority to make ISR is executed in all condition */
    isr_WDT_SetPriority(0);
    
    /* Set up GPIO interrupt and enable it */
    // isr_GPIO_StartEx(GPIOIsrHandler);
​
    /* enable global interrupt */
    CyGlobalIntEnable; 
    
​
    for(;;)
    {
        LED_Status_Write(LIGHT_ON);
        
        /* Delay for LED visibility */
        CyDelay(100u);
​
        LED_Status_Write(LIGHT_OFF);
​
        /* clear watchdog counter before deep sleep */
        CySysWdtResetCounters(CY_SYS_WDT_COUNTER0_RESET);
        /* reset watchdog counter requires several LFCLK cycles to take effect */
        CyDelayUs(150); 
        /* go to deep sleep mode */
        CySysPmDeepSleep();
        
        /* Sleep and wait for interrupt. */
    }
}
​
​
​
/*******************************************************************************
* Function Name: WDTIsrHandler
********************************************************************************
* Summary:
*  The interrupt handler for WDT interrupts.
*  Clears a pending Interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(WDTIsrHandler)
{
    /* clear interrupt flag to enable next interrupt */
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);     
}
​
​
/*******************************************************************************
* Function Name: InitWatchdog
********************************************************************************
* Summary:
*   Initialize watchdog counter0 with specific sleep interval parameter
*
* Parameters:  
*   uint16 sleep_interval   reset interval, in millisecond unit
*
* Return: 
*   void
*
*******************************************************************************/
void InitWatchdog(uint16 sleep_interval)
{
    /*==============================================================================*/
    /* configure counter 0 for wakeup interrupt                                     */
    /*==============================================================================*/
    /* Counter 0 of Watchdog time generates peridically interrupt to wakeup system */
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
    /* Set interval as desired value */
	CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, ((uint32)(sleep_interval * ILO_FREQ) / 1000));
    /* clear counter on match event */
	CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER0, 1u);
    
    /*==============================================================================*/
    /* enable watchdog                                                              */
    /*==============================================================================*/
    /* enable the counter 0 */
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    /* check if counter 0 is enabled, otherwise keep looping here */
    while(!CySysWdtReadEnabledStatus(CY_SYS_WDT_COUNTER0));
}
​
​
/* [] END OF FILE */
