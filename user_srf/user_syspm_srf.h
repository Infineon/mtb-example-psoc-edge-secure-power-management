/***************************************************************************//**
* \file user_syspm_srf.h
* \version 1.000
*
* This file provides macros and exposes operations of USER SYSPM.
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
*
*******************************************************************************/

#pragma once

#include "mtb_srf.h"
#include "cycfg_ppc.h"

#if !defined(COMPONENT_SECURE_DEVICE)
/* Specifiy SYSPM SRF pool timeout value in microseconds. */
#define CY_USER_SYSPM_SRF_POOL_TIMEOUT       0UL
#endif /* !defined(COMPONENT_SECURE_DEVICE) */

/** The USER SysPm function return value status definitions. */
typedef enum
{
    CY_USER_SYSPM_SUCCESS         = 0x00U,    /**< Successful. */
    CY_USER_SYSPM_BAD_PARAM       = 0x01U,    /**< One or more invalid parameters. */
    CY_USER_SYSPM_FAIL            = 0xFFU     /**< Unknown failure. */
} cy_en_user_syspm_status_t;

/** SYSPM Secure Operation IDs */
typedef enum
{
    CY_USER_SYSPM_OP_ENTERHIGHPERFORMANCE,  /**< Cy_USER_SysEnterHp */
    CY_USER_SYSPM_OP_ENTERLOWPOWER,         /**< Cy_USER_SysEnterLp */
    CY_USER_SYSPM_OP_ENTERULTRALOWPOWER,    /**< Cy_USER_SysEnterUlp */
    CY_USER_SYSPM_OP_MAX
} cy_user_syspm_srf_op_id_t;


#if defined(COMPONENT_SECURE_DEVICE)
/** Array of SYSPM Secure Operations */
extern mtb_srf_op_s_t _cy_user_syspm_srf_operations[(uint32_t)CY_USER_SYSPM_OP_MAX];
#endif /* defined(COMPONENT_SECURE_DEVICE) */

/*******************************************************************************
* Function Name: Cy_USER_SysEnterHp
****************************************************************************//**
*
* Sets device into System High Performance mode.
*
* \param none

* \return
* Status of the request.
*
*******************************************************************************/
cy_en_user_syspm_status_t Cy_USER_SysEnterHp(void);

/*******************************************************************************
* Function Name: Cy_USER_SysEnterLp
****************************************************************************//**
*
* Sets device into System Low Power mode.
*
* \param none

* \return
* Status of the request.
*
*******************************************************************************/
cy_en_user_syspm_status_t Cy_USER_SysEnterLp(void);

/*******************************************************************************
* Function Name: Cy_USER_SysEnterUlp
****************************************************************************//**
*
* Sets device into System Ultra Low Performance mode.
*
* \param none

* \return
* Status of the request.
*
*******************************************************************************/
cy_en_user_syspm_status_t Cy_USER_SysEnterUlp(void);
