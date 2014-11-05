/* $Id: xemc_selftest.c,v 1.3 2002/05/14 18:14:39 moleres Exp $ */
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
* @file xemc_selftest.c
*
* The implementation of the XEmc component for the self-test functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a rmm  02/08/02 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xemc.h"
#include "xemc_i.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
*
* Runs a self-test on the driver/device. This includes the following tests:
*
*   - Control register read/write access for each bank.
*
* Memory devices controlled by this component are not changed. However access
* speeds are toggled which could possibly have undesirable effects.
*
* @param    InstancePtr is a pointer to the XEmc instance to be worked on.
*           This parameter must have been previously initialized with
*           XEmc_Initialize().
*
* @return
* <pre>
* XST_SUCCESS     If test passed
*
* XST_FAILURE     If test failed
* </pre>
* @note
*
* - Control register contents are restored to their original state when the
*   test completes.
* - This test does not abort if an error is detected.
*
******************************************************************************/
XStatus XEmc_SelfTest(XEmc *InstancePtr)
{
    int CurrentBank;
    Xuint32 ControlRegSave;
    Xuint32 ControlReg;
    XStatus Rc = XST_SUCCESS;

    /*
     * ASSERT the arguments
     */
    XASSERT_NONVOID(InstancePtr != XNULL);
    XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);

    /*
     * For each bank.
     *   - Save the current contents of the control register.
     *   - Set each control bit and verify.
     *   - Clear each control bit and verify.
     *   - Restore control register to original contents
     */
    for (CurrentBank = 0; CurrentBank < InstancePtr->NumBanks; CurrentBank++)
    {
        /* Save current control register settings */
        ControlRegSave =
                XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);

        /* Set control register to 0 and verify */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank, 0);
        ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);
        if (ControlReg != 0)
        {
            Rc = XST_FAILURE;
        }

        /* Set FS bit and verify */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank,
                            XEMC_CR_FS_ENABLE);
        ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);
        if (ControlReg != XEMC_CR_FS_ENABLE)
        {
            Rc = XST_FAILURE;
        }

        /* Clear FS bit and verify */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank, 0);
        ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);
        if (ControlReg != 0)
        {
            Rc = XST_FAILURE;
        }

        /* Set PM bit and verify */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank,
                            XEMC_CR_PM_ENABLE);
        ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);
        if (ControlReg != XEMC_CR_PM_ENABLE)
        {
            Rc = XST_FAILURE;
        }

        /* Clear PM bit and verify */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank, 0);
        ControlReg = XEmc_mGetControlReg(InstancePtr->RegBaseAddr, CurrentBank);
        if (ControlReg != 0)
        {
            Rc = XST_FAILURE;
        }

        /* Restore control register settings */
        XEmc_mSetControlReg(InstancePtr->RegBaseAddr, CurrentBank,
                            ControlRegSave);
    }

    return(Rc);
}
