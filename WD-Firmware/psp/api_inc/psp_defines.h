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
* @file   psp_defines.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The psp defines
* 
*/
#ifndef  __PSP_DEFINES_H__
#define  __PSP_DEFINES_H__

/**
* include files
*/

/**
* definitions
*/

/* there are 1000 msec in one second */
#define D_PSP_MSEC       1000

/* function return codes */
#define D_PSP_SUCCESS    0x00
#define D_PSP_FAIL       0x01
#define D_PSP_PTR_ERROR  0x02

#define D_PSP_TRUE       1
#define D_PSP_FALSE      0

#define D_PSP_ON         1
#define D_PSP_OFF        0

#define D_PSP_MULT_BY_4(val) ((val) << 2)


#endif /* __PSP_DEFINES_H__ */
