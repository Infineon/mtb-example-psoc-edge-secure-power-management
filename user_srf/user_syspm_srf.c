/***************************************************************************//**
* \file user_syspm_srf.c
* \version 1.000
*
* This file provides source code for Secure Aware USER SYSPM APIs.
*
********************************************************************************
* \copyright
* Copyright (c) (2016-2025), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cy_device.h"
#include "cy_sysclk.h"
#include "cy_syspm.h"
#include "mtb_srf.h"
#include "cy_gpio.h"
#include "cybsp.h"
#include "ifx_se_platform.h"

#include "user_srf.h"
#include "user_syspm_srf.h"
/*******************************************************************************
*    Secure Aware Support
*******************************************************************************/
#if defined(COMPONENT_SECURE_DEVICE)

#define DPLL_ENABLE_TIMEOUT_MS        (10000U)
#define DPLL_INTPUT_FREQ_HZ           (24000000U)
#define DPLL_FREQ_HP_HZ               (400000000U)
#define DPLL_FREQ_LP_HZ               (140000000U)
#define DPLL_FREQ_ULP_HZ              (50000000U)

#define UART_HP_DIV                   (86U)
#define UART_LP_DIV                   (30U)
#define UART_ULP_DIV                  (10U)


cy_rslt_t cy_user_syspm_srf_enterhighperformance_impl_s(mtb_srf_input_ns_t* inputs_ns,
                                            mtb_srf_output_ns_t* outputs_ns,
                                            mtb_srf_invec_ns_t* inputs_ptr_ns,
                                            uint8_t inputs_ptr_cnt_ns,
                                            mtb_srf_outvec_ns_t* outputs_ptr_ns,
                                            uint8_t outputs_ptr_cnt_ns)
{
    CY_UNUSED_PARAMETER(inputs_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_cnt_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_cnt_ns);
    cy_rslt_t retVal;

    retVal = Cy_USER_SysEnterHp();

    memcpy(&outputs_ns->output_values[0], &retVal, sizeof(retVal));

    return (cy_rslt_t)CY_RSLT_SUCCESS;
}

cy_rslt_t cy_user_syspm_srf_enterlowpower_impl_s(mtb_srf_input_ns_t* inputs_ns,
                                            mtb_srf_output_ns_t* outputs_ns,
                                            mtb_srf_invec_ns_t* inputs_ptr_ns,
                                            uint8_t inputs_ptr_cnt_ns,
                                            mtb_srf_outvec_ns_t* outputs_ptr_ns,
                                            uint8_t outputs_ptr_cnt_ns)
{
    CY_UNUSED_PARAMETER(inputs_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_cnt_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_cnt_ns);
    cy_rslt_t retVal;

    retVal = Cy_USER_SysEnterLp();

    memcpy(&outputs_ns->output_values[0], &retVal, sizeof(retVal));

    return (cy_rslt_t)CY_RSLT_SUCCESS;
}

cy_rslt_t cy_user_syspm_srf_enterultralowpower_impl_s(mtb_srf_input_ns_t* inputs_ns,
                                            mtb_srf_output_ns_t* outputs_ns,
                                            mtb_srf_invec_ns_t* inputs_ptr_ns,
                                            uint8_t inputs_ptr_cnt_ns,
                                            mtb_srf_outvec_ns_t* outputs_ptr_ns,
                                            uint8_t outputs_ptr_cnt_ns)
{
    CY_UNUSED_PARAMETER(inputs_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_cnt_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_cnt_ns);
    cy_rslt_t retVal;

    retVal = Cy_USER_SysEnterUlp();

    memcpy(&outputs_ns->output_values[0], &retVal, sizeof(retVal));

    return (cy_rslt_t)CY_RSLT_SUCCESS;
}

cy_rslt_t cy_user_syspm_srf_enterdeepsleep_impl_s(mtb_srf_input_ns_t* inputs_ns,
                                            mtb_srf_output_ns_t* outputs_ns,
                                            mtb_srf_invec_ns_t* inputs_ptr_ns,
                                            uint8_t inputs_ptr_cnt_ns,
                                            mtb_srf_outvec_ns_t* outputs_ptr_ns,
                                            uint8_t outputs_ptr_cnt_ns)
{
    CY_UNUSED_PARAMETER(inputs_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_ns);
    CY_UNUSED_PARAMETER(inputs_ptr_cnt_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_ns);
    CY_UNUSED_PARAMETER(outputs_ptr_cnt_ns);
    cy_rslt_t retVal;

    retVal = Cy_USER_SysEnterDS();

    memcpy(&outputs_ns->output_values[0], &retVal, sizeof(retVal));

    return (cy_rslt_t)CY_RSLT_SUCCESS;
}

/* All operations for the SYSPM submodule of the USER module */
mtb_srf_op_s_t _cy_user_syspm_srf_operations[] =
{
    {
        .module_id = MTB_SRF_MODULE_USER,
        .submodule_id = CY_USER_SECURE_SUBMODULE_SYSPM,
        .op_id = CY_USER_SYSPM_OP_ENTERHIGHPERFORMANCE,
        .write_required = false,
        .impl = cy_user_syspm_srf_enterhighperformance_impl_s,
        .input_values_len = 0UL,
        .output_values_len = 0UL,
        .input_len ={ 0UL, 0UL, 0UL },
        .needs_copy = { false, false, false },
        .output_len ={ 0UL, 0UL, 0UL },
        .allowed_rsc = NULL,
        .num_allowed = 0UL,
    },
    {
        .module_id = MTB_SRF_MODULE_USER,
        .submodule_id = CY_USER_SECURE_SUBMODULE_SYSPM,
        .op_id = CY_USER_SYSPM_OP_ENTERLOWPOWER,
        .write_required = false,
        .impl = cy_user_syspm_srf_enterlowpower_impl_s,
        .input_values_len = 0UL,
        .output_values_len = 0UL,
        .input_len ={ 0UL, 0UL, 0UL },
        .needs_copy = { false, false, false },
        .output_len ={ 0UL, 0UL, 0UL },
        .allowed_rsc = NULL,
        .num_allowed = 0UL,
    },
    {
        .module_id = MTB_SRF_MODULE_USER,
        .submodule_id = CY_USER_SECURE_SUBMODULE_SYSPM,
        .op_id = CY_USER_SYSPM_OP_ENTERULTRALOWPOWER,
        .write_required = false,
        .impl = cy_user_syspm_srf_enterultralowpower_impl_s,
        .input_values_len = 0UL,
        .output_values_len = 0UL,
        .input_len ={ 0UL, 0UL, 0UL },
        .needs_copy = { false, false, false },
        .output_len ={ 0UL, 0UL, 0UL },
        .allowed_rsc = NULL,
        .num_allowed = 0UL,
    },
    {
        .module_id = MTB_SRF_MODULE_USER,
        .submodule_id = CY_USER_SECURE_SUBMODULE_SYSPM,
        .op_id = CY_USER_SYSPM_OP_ENTERDEEPSLEEP,
        .write_required = false,
        .impl = cy_user_syspm_srf_enterdeepsleep_impl_s,
        .input_values_len = 0UL,
        .output_values_len = 0UL,
        .input_len ={ 0UL, 0UL, 0UL },
        .needs_copy = { false, false, false },
        .output_len ={ 0UL, 0UL, 0UL },
        .allowed_rsc = NULL,
        .num_allowed = 0UL,
    }
};


#else

void _Cy_USER_SysPm_Invoke_SRF(cy_user_syspm_srf_op_id_t op_id, cy_en_user_syspm_status_t* retval)
{
    cy_rslt_t result;
    CY_UNUSED_PARAMETER(result);

    mtb_srf_invec_ns_t* inVec = NULL;
    mtb_srf_outvec_ns_t* outVec = NULL;
    mtb_srf_output_ns_t* output = NULL;

    result = mtb_srf_pool_allocate(&cy_user_srf_default_pool, &inVec, &outVec, CY_USER_SYSPM_SRF_POOL_TIMEOUT);
    CY_ASSERT_L2(result == CY_RSLT_SUCCESS);

    cy_user_invoke_srf_args invoke_args =
    {
        .inVec = inVec,
        .outVec = outVec,
        .output_ptr = &output,
        .op_id = op_id,
        .submodule_id = CY_USER_SECURE_SUBMODULE_SYSPM,
        .base = NULL,
        .sub_block = 0UL,
        .input_base = NULL,
        .input_len = 0UL,
        .output_base = (uint8_t*)retval,
        .output_len = sizeof(*retval),
        .invec_bases = NULL,
        .invec_sizes = 0UL,
        .outvec_bases = NULL,
        .outvec_sizes = 0UL
    };

    (void)Cy_USER_Invoke_SRF(&invoke_args);

    result = mtb_srf_pool_free(&cy_user_srf_default_pool, inVec, outVec);
    CY_ASSERT_L2(result == CY_RSLT_SUCCESS);

    memcpy(retval, &(output->output_values[0]), sizeof(*retval));
}

#endif /* defined(COMPONENT_SECURE_DEVICE) */

cy_en_user_syspm_status_t Cy_USER_SysEnterHp(void)
{
    cy_en_user_syspm_status_t result = CY_USER_SYSPM_FAIL;

#if defined(COMPONENT_SECURE_DEVICE)

    cy_en_syspm_status_t status;
    cy_stc_pll_config_t dpll =
    {
        .inputFreq = DPLL_INTPUT_FREQ_HZ,
        .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
        .outputFreq = DPLL_FREQ_HP_HZ,
    };

    status = Cy_SysPm_SystemEnterHp();
    if ((CY_SYSPM_SUCCESS == status) && Cy_SysPm_IsSystemHp())
    {
        /** Set the RRAM to HP voltage mode */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_HP);

        /** Increase the PLL frequency to 400MHz*/
        Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
        Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
        Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);

        /** Set the high-frequency clock (CLKHF) divide */
        Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_DIVIDE_BY_2);

        /** Adjust the peripheral clock divider for the debug UART */
        Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM,
                                     CY_SYSCLK_DIV_16_BIT, 1U, UART_HP_DIV);

        result = CY_USER_SYSPM_SUCCESS;
    }

#else

    _Cy_USER_SysPm_Invoke_SRF(CY_USER_SYSPM_OP_ENTERHIGHPERFORMANCE, &result);

#endif /* defined(COMPONENT_SECURE_DEVICE)*/

    return result;
}

cy_en_user_syspm_status_t Cy_USER_SysEnterLp(void)
{
    cy_en_user_syspm_status_t result = CY_USER_SYSPM_FAIL;

#if defined(COMPONENT_SECURE_DEVICE)

    cy_en_syspm_status_t status;
    cy_stc_pll_config_t dpll =
    {
        .inputFreq = DPLL_INTPUT_FREQ_HZ,
        .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
        .outputFreq = DPLL_FREQ_LP_HZ,
    };

    /** Deccrease the PLL frequency to 140MHz*/
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
    Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
    Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);

    status = Cy_SysPm_SystemEnterLp();
    if ((CY_SYSPM_SUCCESS == status) && Cy_SysPm_IsSystemLp())
    {
        /** Set the RRAM to LP voltage mode for lower power consumption */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_LP);

        /** Set the high-frequency clock (CLKHF) divide */
        Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_DIVIDE_BY_2);

        /** Adjust the peripheral clock divider for the debug UART */
        Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM,
                                     CY_SYSCLK_DIV_16_BIT, 1U, UART_LP_DIV);

        result = CY_USER_SYSPM_SUCCESS;
    }

#else

    _Cy_USER_SysPm_Invoke_SRF(CY_USER_SYSPM_OP_ENTERLOWPOWER, &result);

#endif /* defined(COMPONENT_SECURE_DEVICE)*/

    return result;
}

cy_en_user_syspm_status_t Cy_USER_SysEnterUlp(void)
{
    cy_en_user_syspm_status_t result = CY_USER_SYSPM_FAIL;

#if defined(COMPONENT_SECURE_DEVICE)

    cy_en_syspm_status_t status;
    cy_stc_pll_config_t dpll =
    {
        .inputFreq = DPLL_INTPUT_FREQ_HZ,
        .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
        .outputFreq = DPLL_FREQ_ULP_HZ,
    };

    /** Decrease the PLL frequency to 50MHz*/
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
    Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, &dpll);
    Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);

    status = Cy_SysPm_SystemEnterUlp();
    if ((CY_SYSPM_SUCCESS == status) && Cy_SysPm_IsSystemUlp())
    {
        /** Set the RRAM to ULP voltage mode for lower power consumption */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_ULP);

        /** Set the high-frequency clock (CLKHF) to no divide */
        Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_NO_DIVIDE);

        /** Adjust the peripheral clock divider for the debug UART */
        Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM,
                                     CY_SYSCLK_DIV_16_BIT, 1U, UART_ULP_DIV);

        result = CY_USER_SYSPM_SUCCESS;
    }

#else

    _Cy_USER_SysPm_Invoke_SRF(CY_USER_SYSPM_OP_ENTERULTRALOWPOWER, &result);

#endif /* defined(COMPONENT_SECURE_DEVICE)*/

    return result;
}

cy_en_user_syspm_status_t Cy_USER_SysEnterDS(void)
{
    cy_en_user_syspm_status_t result = CY_USER_SYSPM_FAIL;

#if defined(COMPONENT_SECURE_DEVICE)

    cy_en_syspm_status_t status;

    ifx_se_disable(NULL);

    status = Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    if (CY_SYSPM_SUCCESS == status)
    {
        ifx_se_enable(NULL);

        result = CY_USER_SYSPM_SUCCESS;
    }

#else

    _Cy_USER_SysPm_Invoke_SRF(CY_USER_SYSPM_OP_ENTERDEEPSLEEP, &result);

#endif /* defined(COMPONENT_SECURE_DEVICE)*/

    return result;
}
