/* $Id: xemc.c,v 1.7 2002/06/26 20:19:53 linnj Exp $ */
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
* @file xemc.c
*
* The implementation of the XEmc component. See xemc.h for more information
* about the component.
*
* @note
*
* None
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a rmm  01/29/02 First release
* 1.00a rpm  05/14/02 Made configuration table/lookup public
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "xemc.h"
#include "xemc_i.h"
#include "xparameters.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/*****************************************************************************/
/**
*
* Initializes the XEmc instance provided by the caller based on the given
* DeviceID.
*
* @param    InstancePtr is a pointer to an XEmc instance. The memory the
*           pointer references must be pre-allocated by the caller. Further
*           calls to manipulate the component through the XEmc API must be
*           made with this pointer.
* @param    DeviceId is the unique id of the device controlled by this XEmc
*           component. Passing in a device id associates the generic XEmc
*           instance to a specific device, as chosen by the caller or
*           application developer.
*
* @return
*
* - XST_SUCCESS           Initialization was successful.
* - XST_DEVICE_NOT_FOUND  Device configuration data was not found for a device
*                         with the supplied device ID.
* @note
*
* The control registers for each bank are not modified because it is
* possible that they have been setup during bootstrap processing prior to
* "C" runtime support.
*
*****************************************************************************/
XStatus XEmc_Initialize(XEmc* InstancePtr, Xuint16 DeviceId)
{
    XEmc_Config *ConfigPtr;

    /*
     * Assert arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);

    /*
     * Lookup configuration data in the device configuration table.
     * Use this configuration info down below when initializing this component.
     */
    ConfigPtr = XEmc_LookupConfig(DeviceId);

    if (ConfigPtr == (XEmc_Config *)XNULL)
    {
        InstancePtr->IsReady = 0;
        return(XST_DEVICE_NOT_FOUND);
    }

    /*
     * Set some default values.
     */
    InstancePtr->RegBaseAddr = ConfigPtr->RegBaseAddr;
    InstancePtr->NumBanks = ConfigPtr->NumBanks;

    /*
     * Indicate the instance is now ready to use, initialized without error
     */
    InstancePtr->IsReady = XCOMPONENT_IS_READY;
    return(XST_SUCCESS);
}


/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. The table
* XEmc_ConfigTable contains the configuration info for each device in the
* system.
*
* @param    DeviceID is the device identifier to lookup.
*
* @return
*
* XEmc configuration structure pointer if DeviceID is found.
*
* XNULL if DeviceID is not found.
*
******************************************************************************/
XEmc_Config *XEmc_LookupConfig(Xuint16 DeviceId)
{
    XEmc_Config *CfgPtr = XNULL;

    int i;

    for (i=0; i < XPAR_XEMC_NUM_INSTANCES; i++)
    {
        if (XEmc_ConfigTable[i].DeviceId == DeviceId)
        {
            CfgPtr = &XEmc_ConfigTable[i];
            break;
        }
    }

    return CfgPtr;
}


/*****************************************************************************/
/**
*
* Sets the page mode for the given bank of memory devices.
*
* @param    InstancePtr is a pointer to the XEmc instance to be worked on.
* @param    Bank is the set of devices to change. Valid range is 0 to the
*           number of banks minus one. The number of banks is defined as a
*           constant in xparameters.h (XPAR_EMC_<n>_NUM_BANKS) or it can be
*           found in the NumBanks attribute of the XEmc instance.
* @param    Mode is the new mode to set. Valid modes are XEMC_PAGE_MODE_ENABLE
*           and XEMC_PAGE_MODE_DISABLE.
*
* @return
*
* - XST_SUCCESS Mode successfully set.
* - XST_INVALID_PARAM Mode parameter is invalid.
*
* @note
*
* none
******************************************************************************/
XStatus XEmc_SetPageMode(XEmc *InstancePtr, unsigned Bank, unsigned Mode)
{
    Xuint32 ControlReg;

    /*
     * ASSERT on arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);
    XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
    XASSERT_NONVOID(InstancePtr->NumBanks > Bank);
    XASSERT_NONVOID((Mode != XEMC_PAGE_MODE_ENABLE) &&
                    (Mode != XEMC_PAGE_MODE_DISABLE));

    /*
     * Read control register
     */
    ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, Bank);

    /*
     * Set according to given mode
     */
    switch (Mode)
    {
        case XEMC_PAGE_MODE_ENABLE:
            ControlReg |= XEMC_CR_PM_ENABLE;
            break;

        case XEMC_PAGE_MODE_DISABLE:
            ControlReg &= ~XEMC_CR_PM_ENABLE;
            break;

        default:
            return(XST_INVALID_PARAM);
    }

    /*
     * Write new control register contents.
     */
    XEmc_mSetControlReg(InstancePtr->RegBaseAddr, Bank, ControlReg);
    return(XST_SUCCESS);
}


/*****************************************************************************/
/**
*
* Sets the access speed for the given bank of memory devices.
*
* @param    InstancePtr is a pointer to the XEmc instance to be worked on.
* @param    Bank is the set of devices to change. Valid range is 0 to the
*           number of banks minus one. The number of banks is defined as a
*           constant in xparameters.h (XPAR_EMC_<n>_NUM_BANKS) or it can be
*           found in the NumBanks attribute of the XEmc instance.
* @param    Speed is the new access speed. Valid speeds are XEMC_ACCESS_SPEED_SLOW
*           and XEMC_ACCESS_SPEED_FAST.
*
* @return
*
* - XST_SUCCESS Access speed successfully set.
* - XST_INVALID_PARAM Speed parameter is invalid.
*
* @note
*
* none
******************************************************************************/
XStatus XEmc_SetAccessSpeed(XEmc *InstancePtr, unsigned Bank, unsigned Speed)
{
    Xuint32 ControlReg;

    /*
     * ASSERT on arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);
    XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
    XASSERT_NONVOID(InstancePtr->NumBanks > Bank);
    XASSERT_NONVOID((Speed != XEMC_ACCESS_SPEED_FAST) &&
                    (Speed != XEMC_ACCESS_SPEED_SLOW));

    /*
     * Read control register
     */
    ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, Bank);

    /*
     * Set according to given mode
     */
    switch (Speed)
    {
        case XEMC_ACCESS_SPEED_FAST:
            ControlReg |= XEMC_CR_FS_ENABLE;
            break;

        case XEMC_ACCESS_SPEED_SLOW:
            ControlReg &= ~XEMC_CR_FS_ENABLE;
            break;

        default:
            return(XST_INVALID_PARAM);
    }

    /*
     * Write new control register contents.
     */
    XEmc_mSetControlReg(InstancePtr->RegBaseAddr, Bank, ControlReg);
    return(XST_SUCCESS);
}


/*****************************************************************************/
/**
*
* Gets the current page mode setting for the given bank of memory devices.
*
* @param    InstancePtr is a pointer to the XEmc instance to be worked on.
* @param    Bank is the set of devices to retrieve the setting for. Valid range
*           is 0 to the number of banks minus one. The number of banks is
*           defined as a constant in xparameters.h (XPAR_EMC_<n>_NUM_BANKS)
*           or it can be found in the NumBanks attribute of the XEmc instance.
*
* @return   Current mode of bank. XEMC_PAGE_MODE_ENABLE or XEMC_PAGE_MODE_DISABLE.
*
* @note
*
* none
******************************************************************************/
unsigned XEmc_GetPageMode(XEmc *InstancePtr, unsigned Bank)
{
    /*
     * ASSERT on arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);
    XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
    XASSERT_NONVOID(InstancePtr->NumBanks > Bank);

    /*
     * Read control register & return appropriate value
     */
    if (XEmc_mGetControlReg(InstancePtr->RegBaseAddr, Bank) & XEMC_CR_PM_ENABLE)
    {
        return(XEMC_PAGE_MODE_ENABLE);
    }
    else
    {
        return(XEMC_PAGE_MODE_DISABLE);
    }
}

/*****************************************************************************/
/**
*
* Gets current access speed setting for the given bank of memory devices.
*
* @param    InstancePtr is a pointer to the XEmc instance to be worked on.
* @param    Bank is the set of devices to retrieve the setting for. Valid range
*           is 0 to the number of banks minus one. The number of banks is
*           defined as a constant in xparameters.h (XPAR_EMC_<n>_NUM_BANKS) or
*           it can be found in the NumBanks attribute of the XEmc instance.
*
* @return   Current access speed of bank. XEMC_ACCESS_SPEED_FAST or
*           XEMC_ACCESS_SPEED_SLOW.
*
* @note
*
* none
******************************************************************************/
unsigned XEmc_GetAccessSpeed(XEmc *InstancePtr, unsigned Bank)
{
    /*
     * ASSERT on arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);
    XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
    XASSERT_NONVOID(InstancePtr->NumBanks > Bank);

    /*
     * Read control register & return appropriate value
     */
    if (XEmc_mGetControlReg(InstancePtr->RegBaseAddr, Bank) & XEMC_CR_FS_ENABLE)
    {
        return(XEMC_ACCESS_SPEED_FAST);
    }
    else
    {
        return(XEMC_ACCESS_SPEED_SLOW);
    }
}
