/***************************************************************************//**
* \file user_srf.h
* \version 1.000
*
* This file contains the SRF USER module registration and information on the
* sub-modules.
*
********************************************************************************
* \copyright
* Copyright (c) (2025), Cypress Semiconductor Corporation (an Infineon company) or
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
*
*******************************************************************************/

#pragma once

#include "mtb_srf.h"
#include "cy_syslib.h"
#include "cycfg_ppc.h"

#include "user_syspm_srf.h"

#if !defined(COMPONENT_SECURE_DEVICE)
#include "mtb_srf_pool.h"
extern mtb_srf_pool_t cy_user_srf_default_pool;

/** A struct containing information required to pass through the SRF, specifically through
 * the _Cy_USER_Invoke_SRF API. */
typedef struct
{
    mtb_srf_invec_ns_t*     inVec;          /**< An array of input vectors */
    mtb_srf_outvec_ns_t*    outVec;         /**< An array of output vectors */
    mtb_srf_output_ns_t**   output_ptr;     /**< A double pointer for the helper to set to the output structure */
    uint8_t                 op_id;          /**< The operation ID, e.g. CY_USER_SYSPM_OP_ENTERLOWPOWER */
    uint32_t                submodule_id;   /**< The USER submodule, see cy_user_submodule_t. */
    void*                   base;           /**< The base address of the hardware resource the request is for.
                                                 Set to NULL if not needed */
    uint32_t                sub_block;      /**< The sub-block of the hardware resource the request is for.  Set
                                                 to 0 if not needed. */
    uint8_t*                input_base;     /**< Request input_values */
    size_t                  input_len;      /**< Length of the request input_values */
    uint8_t*                output_base;    /**< Output struct output_values */
    size_t                  output_len;     /**< Length of the output struct's output_values */
    void**                  invec_bases;    /**< Base addresses of inVec[1,2], as needed.  Set to NULl if not needed. */
    size_t*                 invec_sizes;    /**< Size of inVec[1,2]. Ignored if invec_bases is NULL. */
    void**                  outvec_bases;   /**< Base addresses of outVec[1,2], as needed.  Set to NULl if not needed. */
    size_t*                 outvec_sizes;   /**< Size of outVec[1]. Ignored if outvec1_base is NULL. */
} cy_user_invoke_srf_args;

#endif

/* Ordering in this enum must exactly match ordering in cy_user_srf_operations and cy_user_srf_num_operations*/
typedef enum
{
    CY_USER_SECURE_SUBMODULE_SYSPM,
    CY_USER_SECURE_SUBMODULE_END, /* One greater than max submodule */
} cy_user_submodule_t;

/** \} group_user_srf_enums */

#if defined(COMPONENT_SECURE_DEVICE)
/*******************************************************************************
* Function Name: cy_user_srf_module_register
****************************************************************************//**
*
* Registers the USER module with an SRF context.
*
* \param context
* The SRF context with which the USER is registered with

* \return
* Status of the registration request.
*
*******************************************************************************/
cy_rslt_t cy_user_srf_module_register(mtb_srf_context_s_t* context);

#else

/*******************************************************************************
* Function Name: cy_user_srf_module_pool_init
****************************************************************************//**
*
* Initializes the USER module memory pool.
*
* \param none

* \return
* Status of the initialization request.
*
*******************************************************************************/
cy_rslt_t cy_user_srf_module_pool_init(void);

/*******************************************************************************
* Function Name: Cy_USER_Invoke_SRF
****************************************************************************//**
*
* Invokes SRF for USER module.
*
* \param args Arguments with which SRF needs to be invoked

* \return
* Status of the request.
*
*******************************************************************************/
cy_rslt_t Cy_USER_Invoke_SRF(cy_user_invoke_srf_args* args);

#endif /* defined(COMPONENT_SECURE_DEVICE)*/
