/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for CM33 Secure Project.
*
* Related Document: See README.md
*
*
*******************************************************************************
 * (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
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

#include "cy_pdl.h"
#include "cybsp.h"

#include "user_srf.h"

/*****************************************************************************
* Macros
******************************************************************************/
#define CM33_NS_APP_BOOT_ADDR               (CYMEM_CM33_0_m33_nvm_START + CYBSP_MCUBOOT_HEADER_SIZE)
#define DPLL_FREQ_BEFORE_TRIM_HP_TO_LP_HZ   (75000000U)
#define DPLL_FREQ_BEFORE_TRIM_LP_TO_ULP_HZ  (41000000U)
#define DPLL_FREQ_BEFORE_TRIM_ULP_TO_LP_HZ  (41000000U)
#define DPLL_FREQ_BEFORE_TRIM_LP_TO_HP_HZ   (75000000U)

#define DPLL_ENABLE_TIMEOUT_MS              (10000U)
#define DPLL_INTPUT_FREQ_HZ                 (24000000U)
#define DPLL_FREQ_HP_HZ                     (400000000U)
#define DPLL_FREQ_LP_HZ                     (120000000U)
#define DPLL_FREQ_ULP_HZ                    (50000000U)

/* Power Management Callback Functions */
cy_en_syspm_status_t user_syspm_hp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);
cy_en_syspm_status_t user_syspm_lp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);
cy_en_syspm_status_t user_syspm_ulp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);

/* Callback parameter structures */
static cy_stc_syspm_callback_params_t user_syspm_hp_params = {NULL, NULL};
static cy_stc_syspm_callback_params_t user_syspm_lp_params = {NULL, NULL};
static cy_stc_syspm_callback_params_t user_syspm_ulp_params = {NULL, NULL};

/* Callback structures for different power modes */
static cy_stc_syspm_callback_t user_syspm_hp_cb =
{
    .callback = &user_syspm_hp_callback,
    .type = CY_SYSPM_HP,
    .skipMode = 0U,
    .callbackParams = &user_syspm_hp_params,
    .prevItm = NULL,
    .nextItm = NULL,
    .order = 0U
};

static cy_stc_syspm_callback_t user_syspm_lp_cb =
{
    .callback = &user_syspm_lp_callback,
    .type = CY_SYSPM_LP,
    .skipMode = 0U,
    .callbackParams = &user_syspm_lp_params,
    .prevItm = NULL,
    .nextItm = NULL,
    .order = 0U
};

static cy_stc_syspm_callback_t user_syspm_ulp_cb =
{
    .callback = &user_syspm_ulp_callback,
    .type = CY_SYSPM_ULP,
    .skipMode = 0U,
    .callbackParams = &user_syspm_ulp_params,
    .prevItm = NULL,
    .nextItm = NULL,
    .order = 0U
};

/* HP Power Management Callback Implementation */
cy_en_syspm_status_t user_syspm_hp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    CY_UNUSED_PARAMETER(callbackParams);

    if (mode == CY_SYSPM_BEFORE_TRANSITION)
    {
        /* HP Before transition logic */
        cy_stc_pll_config_t dpll = {
            .inputFreq = DPLL_INTPUT_FREQ_HZ,
            .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
            .outputFreq = DPLL_FREQ_BEFORE_TRIM_LP_TO_HP_HZ,
        };

        /** Increase the PLL frequency to 75MHz before transition*/
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    }
    else if (mode == CY_SYSPM_AFTER_TRANSITION)
    {
        /* HP After transition logic */
        cy_stc_pll_config_t dpll = {
            .inputFreq = DPLL_INTPUT_FREQ_HZ,
            .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
            .outputFreq = DPLL_FREQ_HP_HZ,
        };
        /** Set the RRAM to HP voltage mode */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_HP);
        /** Increase the PLL frequency to 400MHz */
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    }

    return CY_SYSPM_SUCCESS;
}

/* LP Power Management Callback Implementation */
cy_en_syspm_status_t user_syspm_lp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    CY_UNUSED_PARAMETER(callbackParams);

    if (mode == CY_SYSPM_BEFORE_TRANSITION)
    {
        if( Cy_SysPm_IsSystemUlp() )
        {
            /* LP Before transition logic */
            cy_stc_pll_config_t dpll = {
                .inputFreq = DPLL_INTPUT_FREQ_HZ,
                .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
                .outputFreq = DPLL_FREQ_BEFORE_TRIM_ULP_TO_LP_HZ,
            };
            /** Increase the PLL frequency to 41MHz before transition*/
            Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
            Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
            Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
        }
        else
        {
            /* LP Before transition logic */
            cy_stc_pll_config_t dpll = {
                .inputFreq = DPLL_INTPUT_FREQ_HZ,
                .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
                .outputFreq = DPLL_FREQ_BEFORE_TRIM_HP_TO_LP_HZ,
            };
            /** Increase the PLL frequency to 75MHz before transition*/
            Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
            Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
            Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
        }
    }
    else if (mode == CY_SYSPM_AFTER_TRANSITION)
    {
        /* LP After transition logic */
        cy_stc_pll_config_t dpll = {
            .inputFreq = DPLL_INTPUT_FREQ_HZ,
            .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
            .outputFreq = DPLL_FREQ_LP_HZ,
        };
        /** Set the RRAM to LP voltage mode for lower power consumption */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_LP);
        /** Decrease the PLL frequency to 120MHz after transition*/
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    }

    return CY_SYSPM_SUCCESS;
}

/* ULP Power Management Callback Implementation */
cy_en_syspm_status_t user_syspm_ulp_callback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    CY_UNUSED_PARAMETER(callbackParams);

    if (mode == CY_SYSPM_BEFORE_TRANSITION)
    {
        /* ULP Before transition logic */
        cy_stc_pll_config_t dpll = {
            .inputFreq = DPLL_INTPUT_FREQ_HZ,
            .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
            .outputFreq = DPLL_FREQ_BEFORE_TRIM_LP_TO_ULP_HZ,
        };
        /** Increase the PLL frequency to 41MHz before transition*/
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    }
    else if (mode == CY_SYSPM_AFTER_TRANSITION)
    {
        /* ULP After transition logic */
        cy_stc_pll_config_t dpll = {
            .inputFreq = DPLL_INTPUT_FREQ_HZ,
            .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
            .outputFreq = DPLL_FREQ_ULP_HZ,
        };
        /** Set the RRAM to ULP voltage mode for lowest power consumption */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_ULP);
        /** Decrease the PLL frequency to 50MHz */
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    }

    return CY_SYSPM_SUCCESS;
}

/*****************************************************************************
* Function Name: main
******************************************************************************
* This is the main function for Cortex M33 CPU secure application
* NOTE: CM33 secure project assumes that certain memory and peripheral regions
* will be accessed from non-secure environment by the CM33 NS /CM55 code,
* For such regions MPC and PPC configurations are applied by cybsp_init to make
* it non-secure. Any access to these regions from the secure side is recommended
* to be done before the MPC/PPC configuration is applied.Once a memory or
* peripheral region is marked as non secure it cannot be accessed from the secure
* side using secure aliased address but may be accessed using non secure aliased
* address
*****************************************************************************/
int main(void)
{
    uint32_t ns_stack;
    cy_cmse_funcptr NonSecure_ResetHandler;
    cy_rslt_t result;

    /* Set up internal routing, pins, and clock-to-peripheral connections */
    result = cybsp_init();

    /* Board initialization failed. Stop program execution */
    if (CY_RSLT_SUCCESS != result)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);

    }

    /* Register SRF USER module */
    result = cy_user_srf_module_register(&cybsp_srf_context);
    if (result != CY_RSLT_SUCCESS)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);
    }

    /* Enable global interrupts */
    __enable_irq();

    ns_stack = (uint32_t)(*((uint32_t*)CM33_NS_APP_BOOT_ADDR));
    __TZ_set_MSP_NS(ns_stack);

    NonSecure_ResetHandler = (cy_cmse_funcptr)(*((uint32_t*)(CM33_NS_APP_BOOT_ADDR + 4)));

    /* Register the HP transition callback */
    Cy_SysPm_RegisterCallback(&user_syspm_hp_cb);

    /* Register the LP transition callback */
    Cy_SysPm_RegisterCallback(&user_syspm_lp_cb);

    /* Register the ULP transition callback */
    Cy_SysPm_RegisterCallback(&user_syspm_ulp_cb);

    /* System Domain Idle Power Mode Configuration */
    result = (cy_rslt_t)Cy_SysPm_SetDeepSleepMode(CY_SYSPM_MODE_DEEPSLEEP);
    if (result != CY_RSLT_SUCCESS)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);
    }

    /* App Domain Idle Power Mode Configuration */
    result = (cy_rslt_t)Cy_SysPm_SetAppDeepSleepMode(CY_SYSPM_MODE_DEEPSLEEP);
    if (result != CY_RSLT_SUCCESS)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);
    }

    /* System SRAM (SoCMEM) Idle Power Mode Configuration */
    result = (cy_rslt_t)Cy_SysPm_SetSOCMEMDeepSleepMode(CY_SYSPM_MODE_DEEPSLEEP);
    if (result != CY_RSLT_SUCCESS)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);
    }

    /* Start non-secure application */
    NonSecure_ResetHandler();

    for (;;)
    {
    }
}

/* [] END OF FILE */
