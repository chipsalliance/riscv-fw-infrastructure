/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file   psp_interrupts_swerv_eh1.c
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file supllies interrups services such as  - registration, default handlers and indication
*         whether we are in interrpu context.
*         The file is specific to swerv_eh1 specifications
*
*/

/**
* include files
*/
#include "psp_types.h"
#include "psp_api.h"
#include "mem_map.h"
#include "version.h"

/**
* definitions
*/

/* The stack used by interrupt service routines */


/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
 *
* The function return version num
*
* @param inputs: ucRev, minor, major, sha, dirty
*
*/
void versionGetSwervolfVer(swervolfVersion_t *pSwervolfVersion)
{
  pSwervolfVersion->ucRev   = M_PSP_READ_REGISTER_32(D_VERSION_REV);
  pSwervolfVersion->ucMinor = M_PSP_READ_REGISTER_32(D_VERSION_MINOR);
  pSwervolfVersion->ucMajor = M_PSP_READ_REGISTER_32(D_VERSION_MAJOR);
  pSwervolfVersion->ucSha   = M_PSP_READ_REGISTER_32(D_VERSION_SHA);
  pSwervolfVersion->ucDirty = M_PSP_READ_REGISTER_32(D_VERSION_DIRTY);
}
