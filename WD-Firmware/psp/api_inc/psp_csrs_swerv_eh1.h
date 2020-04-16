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
* @file   psp_csrs_swerv_eh1.h
* @author Nati Rapaport
* @date   01.12.2019
* @brief  Definitions of Swerv's (EH1 version) CSRs
* 
*/
#ifndef  __PSP_CSRS_SWERV_EH1_H__
#define  __PSP_CSRS_SWERV_EH1_H__

/**
* include files
*/

/**
* definitions
*/
/* PIC base address - for memory-mapped CSRs */
#if (0 == D_PIC_BASE_ADDRESS)
    #error "Definition of PIC base-address is missing"
#else
    #define PSP_PIC_BASE_ADDRESS D_PIC_BASE_ADDRESS
#endif

/** Non standard CSRs in SweRV first-generation cores   **/
#define		D_PSP_MRAC_NUM       0x7C0 	/* Region access control  */
#define		D_PSP_MCPC_NUM       0x7C2 	/* Core pause control  */
#define		D_PSP_DMST_NUM       0x7C4 	/* Memory synchronization trigger (debug mode only)  */
#define		D_PSP_MPMC_NUM       0x7C6 	/* Power management control  */
#define		D_PSP_DICAWICS_NUM   0x7C8 	/* I-cache array/way/index selection (debug mode only)  */
#define		D_PSP_DICAD0_NUM     0x7C9 	/* I-cache array data 0 (debug mode only)  */
#define		D_PSP_DICAD1_NUM     0x7CA 	/* I-cache array data 1 (debug mode only)  */
#define		D_PSP_DICAGO_NUM     0x7CB 	/* I-cache array go (debug mode only)  */
#define		D_PSP_MGPMC_NUM      0x7D0 	/* Group performance monitor control  */
#define		D_PSP_MICECT_NUM     0x7F0 	/* I-cache error counter/threshold */
#define		D_PSP_MICCMECT_NUM   0x7F1 	/* ICCM correctable error counter/threshold  */
#define		D_PSP_MDCCMECT_NUM   0x7F2 	/* DCCM correctable error counter/threshold  */
#define		D_PSP_MCGC_NUM       0x7F8 	/* Clock gating control  */
#define		D_PSP_MFDC_NUM       0x7F9 	/* Feature disable control  */
#define		D_PSP_MDEAU_NUM      0xBC0 	/* D-Bus error address unlock  */
#define		D_PSP_MDSEAC_NUM     0xFC0 	/* D-bus first error address capture  */



/*************************************/
/* PIC CSRs in the CSR address space */
/*************************************/

/* meipt CSR */
#define	D_PSP_MEIPT_NUM              0xBC9 	    /* External interrupts priority threshold  */
#define D_PSP_MEIPT_PRITHRESH_MASK   0x0000000F /* bits 0..3 */

/* meivt CSR */
#define	D_PSP_MEIVT_NUM              0xBC8 	    /* External interrupts vector table  */
#define D_PSP_MEIVT_BASE_MASK        0xFFFFFC00 /* bits 10..31 */

/* meihap CSR */
#define D_PSP_MEIHAP_NUM             0xFC8      /* External interrupts handler address pointer  */
#define D_PSP_MEIHAP_CLAIMID_MASK    0x000003FC /* bits 2..9   */
#define D_PSP_MEIHAP_BASE_MASK       0xFFFFFC00 /* bits 10..31 */

/* meicpct CSR */
#define D_PSP_MEICPCT_NUM            0xBCA      /* External interrupts claim ID / priority level capture trigger  */

/* meicidpl CSR*/
#define D_PSP_MEICIDPL_NUM           0xBCB      /* External interrupts claim IDs priority level  */
#define D_PSP_MEICIDPL_CLIDPRI_MASK  0x0000000F /* bits 0..3 */

/* meicurpl CSR */
#define D_PSP_MEICURPL_NUM           0xBCC      /* External interrupts current priority level  */
#define D_PSP_MEICURPL_CURRPRI_MASK  0x0000000F /* bits 0..3 */

/*************************************/
/* PIC memory mapped registers       */
/*************************************/

/* meipl CSR */
#define D_PSP_PIC_MEIPL_OFFSET      0x0000
#define D_PSP_MEIPL_ADDR            PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIPL_OFFSET  /* External interrupts priority level */
#define D_PSP_MEIPL_PRIORITY_MASK   0x0000000F /* bits 0..3 */

/* meip CSR */
#define D_PSP_PIC_MEIP_OFFSET		0x1000
#define D_PSP_MEIP_ADDR             PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIP_OFFSET	/* External interrupts pending  */

/* meie CSR */
#define D_PSP_PIC_MEIE_OFFSET		0x2000
#define D_PSP_PIC_MEIE_ADDR         PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIE_OFFSET	/* External interrupts enable register */
#define D_PSP_MEIE_INT_EN_MASK      0x00000001 /* bit 0 */

/* mpiccfg CSR */
#define D_PSP_PIC_MPICCFG_OFFSET	0x3000
#define D_PSP_PIC_MPICCFG_ADDR      PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MPICCFG_OFFSET /* PIC configuration */
#define D_PSP_MPICCFG_PRIORD_MASK   0x00000001 /* bit 0 */

/* meigwctrl CSR */
#define D_PSP_PIC_MEIGWCTRL_OFFSET	         0x4000
#define D_PSP_PIC_MEIGWCTRL_ADDR             PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIGWCTRL_OFFSET	/* External interrupts gateway configuration */
#define D_PSP_MEIGWCTRL_POLARITY_BIT_OFFSET  0 /* bit 0 */
#define D_PSP_MEIGWCTRL_TYPE_BIT_OFFSET      1 /* bit 1 */

/* meigwclr CSR */
#define D_PSP_PIC_MEIGWCLR_OFFSET	0x5000
#define D_PSP_PIC_MEIGWCLR_ADDR     PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIGWCLR_OFFSET	/* External interrupts gateway clear */



/* mgpmc CSR */
#define D_PSP_MGPMC_NUM     0x7D0
#define D_PSP_MGMPC_MASK    0x00000001 /* Performance Monitor enable/disable */

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
* APIs
*/



#endif /* __PSP_CSRS_SWERV_EH1_H__ */
