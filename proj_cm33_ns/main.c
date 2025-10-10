/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for non-secure
*                    application in the CM33 CPU
*
* Related Document : See README.md
*
********************************************************************************
* (c) 2023-2025, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
* 
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/

#include "cybsp.h"
#include "retarget_io_init.h"

#include "user_srf.h"
#include "user_syspm_srf.h"

/*****************************************************************************
* Macros and Structures
*****************************************************************************/

/* LED toggle delay. This delay is applicable/true in POWER_MODE_HP only.
 * This is because, SystemCoreClock variable is not updated when the power mode
 * is changes. This is intentional to show that the core frequency is indeed 
 * getting changed as a result of successful power mode switch. */
#define BLINKY_LED_DELAY_MSEC (500U)

/* The timeout value in microseconds used to wait for CM55 core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR (CYMEM_CM33_0_m55_nvm_START + CYBSP_MCUBOOT_HEADER_SIZE)

/* USER BTN1 interrupt priority */
#define BTN_IRQ_PRIORITY (7U)

/* Helper macro to wait for completion of UART transmission */
#define WAIT_FOR_TX_COMPLETE() while (!(Cy_SCB_UART_IsTxComplete(CYBSP_DEBUG_UART_HW)))

/* Power Modes */
typedef enum
{
    POWER_MODE_HP = 0U,
    POWER_MODE_LP,
    POWER_MODE_ULP,
    POWER_MODE_DS,
    POWER_MODE_MAX
} en_power_mode_t;

/*******************************************************************************
* Global Variables
*******************************************************************************/

static volatile bool user_btn_flag = false;

/*****************************************************************************
* Function Definitions
*****************************************************************************/

/********************************************************************************
 * Function Name: check_status
 ********************************************************************************
 * Summary:
 *  Checks for non-zero status, and prints success/error message accordingly.
 *  Indicates the non-zero status by turning the LED1 on, and asserts the
 *  non-zero status.
 *
 * Parameters:
 *  status - status for evaluation.
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void check_status(cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        /* Print Error message */
        printf(" Status: FAILED [%u]\r\n", (unsigned int)status);
        printf("==========================================================\r\n");

        /* Turn on the LED1*/
        Cy_GPIO_Set(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);
        handle_app_error();
    }
    else
    {
        /* Print Success message */
        printf(" Status: SUCCESS\r\n");
    }
}


/*******************************************************************************
* Function Name: gpio_isr_handler
********************************************************************************
* Summary:
*  Interrupt serive routine for CYBSP_USER_BTN1_PORT and .
*  This function checks which user button was presssed and sets the
*  button_status flag.
*
* Parameter:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void gpio_isr_handler(void)
{
    /* Delay to handle de-bouncing  */
    Cy_SysLib_Delay(200);

    /* Check if USER_BTN1 was pressed */
    if(1UL == Cy_GPIO_GetInterruptStatus(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_PIN))
    {
        user_btn_flag = true;
       
        /* Clear the USER_BTN1 interrupt */
        Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_PIN);
    }

    /* CYBSP_USER_BTN1 (SW2) and CYBSP_USER_BTN2 (SW4) share the same port in
     * the PSOC™ Edge E84 evaluation kit and hence they share the same NVIC IRQ
     * line. Since both the buttons are configured for falling edge interrupt in
     * the BSP, pressing any button will trigger the execution of this ISR. Therefore,
     * we must clear the interrupt flag of the user button (CYBSP_USER_BTN2) to avoid
     * issues in case if user presses BTN2 by mistake */
#if defined(CYBSP_USER_BTN2_ENABLED)
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN2_PORT, CYBSP_USER_BTN2_PIN);
    NVIC_ClearPendingIRQ(CYBSP_USER_BTN2_IRQ);
#endif
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function of the CM33 non-secure application. 
* 
* It initializes the device and board peripherals. It also initializes the 
* retarget-io middleware to be used with the debug UART port using which 
* messages are printed on the debug UART. The LED1 pin is initialized with
* default configurations. The CM55 core is enabled and then the programs enters
* an infinite while loop which toggles the LED1 and waits for BTN1 interrupt to 
* change the Power mode of the device.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    uint32_t loop_cnt = 0;
    en_power_mode_t cur_pwr_mode = POWER_MODE_HP;
    en_power_mode_t nxt_pwr_mode = POWER_MODE_LP;

    /* Interrupt config structure */
    cy_stc_sysint_t gpio_int_config =
    {
        .intrSrc = CYBSP_USER_BTN1_IRQ,
        .intrPriority = BTN_IRQ_PRIORITY
    };

    /* Initialize the device and board peripherals. */
    result = cybsp_init();

    /* Board initialization failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

    /* Initialize SRF user module memory pool */
    cy_user_srf_module_pool_init();
    if (CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io middleware */
    init_retarget_io();

    /* Enable CM55. */
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CY_SYS_CORE_WAIT_INFINITE);

    /* Configure USER_BTN1 interrupt */
    /* CYBSP_USER_BTN1 (SW2) and CYBSP_USER_BTN2 (SW4) share the same port in the
     * PSOC™ Edge E84 evaluation kit and hence they share the same NVIC IRQ line.
     * Since both are configured in the BSP via the Device Configurator, the
     * interrupt flags for both the buttons are set right after they get initialized
     * through the call to cybsp_init(). The flags must be cleared before initializing
     * the interrupt, otherwise the interrupt line will be constantly asserted */
#if defined(CYBSP_USER_BTN2_ENABLED)
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN2_PORT, CYBSP_USER_BTN2_PIN);
    NVIC_ClearPendingIRQ(CYBSP_USER_BTN2_IRQ);
#endif
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_PIN);
    NVIC_ClearPendingIRQ(CYBSP_USER_BTN1_IRQ);
    Cy_SysInt_Init(&gpio_int_config, gpio_isr_handler);
    NVIC_EnableIRQ(gpio_int_config.intrSrc);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen. */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
           "PSOC Edge MCU: Secure Power Management"
           " ******************\r\n\n");

    printf(" Device is currently in High Performance Mode\r\n\n");
    printf(" Press USER BTN1 to change the Power Mode\r\n\n");

    for(;;)
    {
        /* Check and update the power mode */
        if (user_btn_flag)
        {
            printf("==========================================================\r\n");

            switch (nxt_pwr_mode)
            {
                case POWER_MODE_HP:
                {
                    printf(" Switching to High Performance mode\r\n");
                    WAIT_FOR_TX_COMPLETE();

                    result = (cy_rslt_t)Cy_USER_SysEnterHp();
                    check_status(result);

                    nxt_pwr_mode = POWER_MODE_LP;
                    cur_pwr_mode = POWER_MODE_HP;
                }
                break;
                
                case POWER_MODE_LP:
                {
                    printf(" Switching to Low Power mode\r\n");
                    WAIT_FOR_TX_COMPLETE();

                    result = (cy_rslt_t)Cy_USER_SysEnterLp();
                    check_status(result);

                    nxt_pwr_mode = (cur_pwr_mode == POWER_MODE_HP) ? POWER_MODE_ULP : POWER_MODE_HP;
                    cur_pwr_mode = POWER_MODE_LP;
                }
                break;

                case POWER_MODE_ULP:
                {
                    printf(" Switching to Ultra Low Power mode\r\n");
                    WAIT_FOR_TX_COMPLETE();

                    result = (cy_rslt_t)Cy_USER_SysEnterUlp();
                    check_status(result);

                    nxt_pwr_mode = POWER_MODE_DS;
                    cur_pwr_mode = POWER_MODE_ULP;
                }
                break;

                case POWER_MODE_DS:
                {
                    printf(" Entering Deep Sleep mode \r\n\n");
                    WAIT_FOR_TX_COMPLETE();

                    Cy_GPIO_Clr(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);

                    result = (cy_rslt_t)Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
                    check_status(result);

                    /* Device will be back in ULP on successful entry and exit from Deep sleep mode */
                    printf(" Wakeup from Deep Sleep mode, back in Ultra Low Power mode\r\n");
                    nxt_pwr_mode = POWER_MODE_LP;
                    cur_pwr_mode = POWER_MODE_ULP;
                }
                break;

                default:
                {
                    printf(" Error: Invalid Power Mode \r\n\n");
                    CY_ASSERT(0);
                }
                break;
            }

            printf("==========================================================\r\n");

            user_btn_flag = false;
        }

        /* Check and toggle the LED */
        if (loop_cnt == BLINKY_LED_DELAY_MSEC)
        {
            loop_cnt = 0;
            Cy_GPIO_Inv(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);
        }

        loop_cnt++;
        Cy_SysLib_Delay(1U);
    }
}

/* [] END OF FILE */