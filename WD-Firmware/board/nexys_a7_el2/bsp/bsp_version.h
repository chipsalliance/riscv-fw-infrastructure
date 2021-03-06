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
* @author Ofer Shinaar
* @date   24.02.2020
* @brief  Supplies SweRVolf version information
*/
#ifndef __BSP_VERSION_H__
#define __BSP_VERSION_H__

/* version sturct */
typedef struct swervolfVersion
{
  u08_t ucRev;
  u08_t ucMinor;
  u08_t ucMajor;
  u08_t ucMisc;
  u32_t uiSha;

} swervolfVersion_t;

/**
 *
* The function return version num
*
* @param inputs: ucRev, minor, major, sha, dirty
*
*/
void versionGetSwervolfVer(swervolfVersion_t *pSwervolfVersion);

#endif //__BSP_VERSION_H__

