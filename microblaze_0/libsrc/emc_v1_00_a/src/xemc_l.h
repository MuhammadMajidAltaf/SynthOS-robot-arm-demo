/* $Id: xemc_l.h,v 1.1 2002/05/14 18:13:56 moleres Exp $ */
/******************************************************************************
*
*       XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
*       AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
*       SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
*       OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
*       APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
*       THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
*       AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
*       FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
*       WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
*       IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
*       REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
*       INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*       FOR A PARTICULAR PURPOSE.
*
*       (c) Copyright 2002 Xilinx Inc.
*       All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xemc_l.h
*
* Contains identifiers and low-level macros that can be used to access the
* device directly. High-level functions are defined in xemc.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a rpm  05/14/02 First release
* </pre>
*
******************************************************************************/
#ifndef XEMC_L_H /* prevent circular inclusions */
#define XEMC_L_H /* by using protection macros */

/***************************** Include Files *********************************/
#include "xbasic_types.h"
#include "xio.h"

/************************** Constant Definitions *****************************/

/*
 * Register offsets.
 *
 * There is a single control register for each bank.
 *
 * The EMC can be configured with between 1 to 8 banks of memory devices.
 * See xparameters.h for the definition for the number of banks present.
 */
#define XEMC_CR0_OFFSET   0x00000000      /* Control register for bank 0 */
#define XEMC_CR1_OFFSET   0x00000004      /* Control register for bank 1 */
#define XEMC_CR2_OFFSET   0x00000008      /* Control register for bank 2 */
#define XEMC_CR3_OFFSET   0x0000000C      /* Control register for bank 3 */
#define XEMC_CR4_OFFSET   0x00000010      /* Control register for bank 4 */
#define XEMC_CR5_OFFSET   0x00000014      /* Control register for bank 5 */
#define XEMC_CR6_OFFSET   0x00000018      /* Control register for bank 6 */
#define XEMC_CR7_OFFSET   0x0000001C      /* Control register for bank 7 */

/*
 * Masks and bit definitions for the CR register
 */
#define XEMC_CR_FS_ENABLE 0x00000001  /* Fast/slow mode enable.
                                         0-slow,
                                         1-fast */
#define XEMC_CR_PM_ENABLE 0x00000002  /* Page mode enable.
                                         0-page mode disable
                                         1-page mode enable */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/*****************************************************************************
*
* Low-level driver macros and functions. The list below provides signatures
* to help the user use the macros.
*
* Xuint32 XEmc_mGetControlReg(Xuint32 BaseAddress, unsigned Bank)
* void XEmc_mSetControlReg(Xuint32 BaseAddress, unsigned Bank, Xuint32 Data)
*
* void XEmc_mEnablePageMode(Xuint32 BaseAddress, unsigned Bank)
* void XEmc_mDisablePageMode(Xuint32 BaseAddress, unsigned Bank)
* void XEmc_mEnableFastAccess(Xuint32 BaseAddress, unsigned Bank)
* void XEmc_mDisableFastAccess(Xuint32 BaseAddress, unsigned Bank)
*
*****************************************************************************/

/*****************************************************************************/
/**
*
* Calculate the offset of a control register based on its bank. This macro is
* used internally.
*
* @param    Bank is the bank number of the control register offset to calculate
*
* @return   Offset to control register associated with Bank parameter.
*
* @note
*
* - To compute the physical address of the register add the
*   base address of the component to the result of this macro.
* - Does not test for validity of Bank.
* - Macro signature: unsigned XEmc_mGetOffset(unsigned Bank)
*
******************************************************************************/
#define XEmc_mGetOffset(Bank) (XEMC_CR0_OFFSET + (4 * Bank))


/*****************************************************************************/
/**
*
* Reads the contents of a bank's control register.
*
* @param    Base is the base address of the EMC component.
* @param    Bank identifies the control register to read.
*
* @return   Value of the Bank's control register
*
* @note
*
* - Macro signature:
*     Xuint32 XEmc_mGetControlReg(Xuint32 Base, unsigned Bank)
*
******************************************************************************/
#define XEmc_mGetControlReg(Base, Bank) \
    XIo_In32((XIo_Address)((Base) + XEmc_mGetOffset(Bank)))


/*****************************************************************************/
/**
*
* Writes to a bank's control register.
*
* @param    Base is the base address of the EMC component.
* @param    Bank identifies the control register to modify.
* @param    Data is the data to write to the control register.
*
* @return   None.
*
* @note
*
* - Macro signature:
*     void XEmc_mSetControlReg(Xuint32 Base, unsigned Bank, Xuint32 Data)
*
******************************************************************************/
#define XEmc_mSetControlReg(Base, Bank, Data) \
    XIo_Out32((XIo_Address)((Base) + XEmc_mGetOffset(Bank)), (Data))


/****************************************************************************/
/**
*
* Enable page mode in the given memory bank.
*
* @param    BaseAddress is the base address of the device
* @param    Bank is the memory bank to set.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
#define XEmc_mEnablePageMode(BaseAddress, Bank) \
            XEmc_mSetControlReg(BaseAddress, Bank, \
                XEmc_mGetControlReg(BaseAddress, Bank) | XEMC_CR_PM_ENABLE)


/****************************************************************************/
/**
*
* Disable page mode in the given memory bank.
*
* @param    BaseAddress is the base address of the device
* @param    Bank is the memory bank to set.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
#define XEmc_mDisablePageMode(BaseAddress, Bank) \
            XEmc_mSetControlReg(BaseAddress, Bank, \
                XEmc_mGetControlReg(BaseAddress, Bank) & ~XEMC_CR_PM_ENABLE)


/****************************************************************************/
/**
*
* Enable fast access in the given memory bank.
*
* @param    BaseAddress is the base address of the device
* @param    Bank is the memory bank to set.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
#define XEmc_mEnableFastAccess(BaseAddress, Bank) \
            XEmc_mSetControlReg(BaseAddress, Bank, \
                XEmc_mGetControlReg(BaseAddress, Bank) | XEMC_CR_FS_ENABLE)


/****************************************************************************/
/**
*
* Disable fast access in the given memory bank.
*
* @param    BaseAddress is the base address of the device
* @param    Bank is the memory bank to set.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
#define XEmc_mDisableFastAccess(BaseAddress, Bank) \
            XEmc_mSetControlReg(BaseAddress, Bank, \
                XEmc_mGetControlReg(BaseAddress, Bank) & ~XEMC_CR_FS_ENABLE)


/************************** Function Prototypes ******************************/

#endif  /* end of protection macro */
