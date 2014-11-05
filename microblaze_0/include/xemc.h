/* $Id: xemc.h,v 1.5 2002/05/16 17:21:35 moleres Exp $ */
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
* @file xemc.h
*
* This file contains the software API definition of the Xilinx External Memory
* Controller (XEmc) component. This controller can be attached to host OPB or
* PLB buses to control multiple banks of supported memory devices. The type
* of host bus is transparent to software.
*
* This driver allows the user to access the device's registers to support
* fast/slow access to the memory devices as well as enabling/disabling paged
* mode access.
*
* The Xilinx OPB/PLB External memory controller is a soft IP core designed for
* Xilinx FPGAs and contains the following general features:
*   - Support for 128, 64, 32, 16, and 8 bit bus interfaces.
*   - Controls up to 8 banks of supported memory devices.
*   - Separate control register for each bank of memory.
*   - Selectable wait state control (fast or slow). (See note 1)
*   - Supports page mode accesses. Page size is 8 bytes.
*   - System clock frequency of up to 133 MHz.
*
* OPB features:
*   - OPB V2.0 bus interface with byte-enable support.
*   - Memory width of connected devices is the same as or smaller than OPB
*     bus width.
*
* @note
* <pre>
* (1) The number of wait states inserted for fast and slow mode is determined
*     by the HW designer and is hard-coded into the IP. Each bank has its
*     own settings.
*
* (2) For read accesses, fast/slow access mode is ignored when page mode
*     is enabled. For write accesses, page mode does not apply.
*
* (3) This driver is not thread-safe. Thread safety must be guaranteed by
*     the layer above this driver if there is a need to access the device
*     from multiple threads.
*
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a rmm  01/29/02 First release
* 1.00a rpm  05/14/02 Made configuration table/lookup public
* </pre>
*
*****************************************************************************/
#ifndef XEMC_H  /* prevent circular inclusions */
#define XEMC_H  /* by using protection macros */

/***************************** Include Files ********************************/
#include "microblaze_0/include/xbasic_types.h"
#include "microblaze_0/include/xstatus.h"

/************************** Constant Definitions ****************************/

/** @name Page mode options
 *
 * Page modes used in XEmc_SetPageMode().
 * @{
 */
#define XEMC_PAGE_MODE_ENABLE  0
#define XEMC_PAGE_MODE_DISABLE 1
/*@}*/

/** @name Access speed options
 *
 * Access speeds used in XEmc_SetAccessSpeed(). What constitutes fast and
 * slow are defined by the IP design.
 * @{
 */
#define XEMC_ACCESS_SPEED_FAST 2
#define XEMC_ACCESS_SPEED_SLOW 3
/*@}*/

/**************************** Type Definitions ******************************/

/**
 * This typedef contains configuration information for the device.
 */
typedef struct
{
    Xuint16 DeviceId;      /**< Unique ID  of device */
    Xuint32 RegBaseAddr;   /**< Register base address */
    Xuint8  NumBanks;      /**< Number of devices controlled by this component */
} XEmc_Config;


/**
 * The XEmc driver instance data. The user is required to allocate a
 * variable of this type for every EMC device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct
{
    Xuint32 RegBaseAddr;   /* Base address of registers */
    Xuint32 IsReady;       /* Device is initialized and ready */
    Xuint8  NumBanks;      /* Number of device banks under control */
} XEmc;


/***************** Macros (Inline Functions) Definitions ********************/


/************************** Function Prototypes *****************************/

/*
 * Initialization and configuration functions. These functions are mandatory.
 * This API is implemented in xemc.c
 */
XStatus  XEmc_Initialize(XEmc *InstancePtr, Xuint16 DeviceId);
XEmc_Config *XEmc_LookupConfig(Xuint16 DeviceId);

XStatus  XEmc_SetPageMode(XEmc *InstancePtr, unsigned Bank, unsigned Mode);
XStatus  XEmc_SetAccessSpeed(XEmc *InstancePtr, unsigned Bank, unsigned Speed);
unsigned XEmc_GetPageMode(XEmc *InstancePtr, unsigned Bank);
unsigned XEmc_GetAccessSpeed(XEmc *InstancePtr, unsigned Bank);

/*
 * Selftest is optional
 * This API is implemented in xemc_selftest.c
 */
XStatus XEmc_SelfTest(XEmc *InstancePtr);

#endif            /* end of protection macro */
