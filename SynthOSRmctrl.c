/*
 *  * Copyright (c) 2004 Zeidman Technologies, Inc.  All rights reserved.
 */

/*
 * This file was developed and tested under Xilinx EDK 6.3 EDK_Gmm.10
 *
 * This file is a sample test application
 *
 * This application is intended to demonstrate a heterogenous multi
 * processor SynthOS application that control a multi axis robotic arm
 *
 * Your XPS project directory is at:
 *    C:\EDKProjects\DesignCon
 */


#include "microblaze_0/include/xparameters.h"
#include "microblaze_0/include/xutil.h"
#include "microblaze_0/include/xgpio_l.h"
#include "microblaze_0/include/xuartlite_l.h"
#include "pcmouse.h"

static long DeltaX;
static long DeltaY;
//static long DeltaZ;
static int Button;

typedef struct {
	long DeltaX;
	long DeltaY;
	long DeltaZ;
	int Button;
} DataInterface;

static DataInterface *DataInterfacePtr = (DataInterface *)0x01000000;	//This is placed in shared memory in the SDRAM sapce

/*
	PAUSE,
	WRIST_CCW,
	SHOULDER_UP,
	BASE_CCW,
	ELBOW_DN,
	GRIP_CL,	
	WRIST_CW,
	SHOULDER_DN,
	BASE_CW,
	ELBOW_UP,
	GRIP_OP,	
	WRIST_PAUSE,
	SHOULDER_PAUSE,
	BASE_PAUSE,
	ELBOW_PAUSE,
	GRIP_PAUSE,	
	NUM_CMDS
*/


static const int Delay = 1000;

static Xuint8 SynthOS_XUartLite_RecvByte(Xuint32 BaseAddress);

typedef enum {PAUSE, WRIST_CCW, SHOULDER_UP, BASE_CCW, ELBOW_DOWN, GRIPPER_CLOSE, WRIST_CW,
             SHOULDER_DOWN, BASE_CW, ELBOW_UP, GRIPPER_OPEN, WRIST_PAUSE,SHOULDER_PAUSE, 
             BASE_PAUSE, ELBOW_PAUSE, GRIP_PAUSE} ArmCmd ;

/*
* A loopback testing function
*/
void Loopback() {
	while(1) {
		XUartLite_SendByte(XPAR_RS232_BASEADDR, XUartLite_RecvByte(XPAR_RS232_BASEADDR));
	}
}

/*
 * Routine to control the robotic arm
 * Parameters:
 * int Cmd - the command to be sent to the arm
 * int Success - 0 indicate sucess otherwise err number
 */ 
int ArmCmdSend(/*long DeltaX, long DeltaY, int Button*/) {

	//xil_printf("%c", Cmd);	// This can be used instead of the following command.
								// But the following is probably better becase the same command is good for both UARTs
	
	// Buttons handling
	if(Button == MWBUTTON_L)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, GRIPPER_CLOSE);
	else if(Button == MWBUTTON_R)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, GRIPPER_OPEN);
	else
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, PAUSE);

	// Base movement handling
	if(DeltaX > XThreshold)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_CW);
	else if(DeltaX < -XThreshold)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_CCW);
		else
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_PAUSE);

	// Sholder movement handling
	if(DeltaY > YThreshold)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_UP);
	else if(DeltaY < -YThreshold)
		XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_DOWN);
		else
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_PAUSE);

	return 0;

}


#if 0
/*
 * This task reads mouse input
 */
void taskMouseRead() {
 	for(;;) {
		MOU_Read(&DeltaX, &DeltaY, &DeltaZ, &Button);
		DataInterfacePtr->Button = Button;
		DataInterfacePtr->DeltaX = DeltaX;
		DataInterfacePtr->DeltaY = DeltaY;
		SynthOS_sleep();
	}
}
#endif


#if 0
/*
 * This task send an arm command
 */
void taskArmCmd() {
	int ix;
 	for(;;) {
		Button = DataInterfacePtr->Button;
		DeltaX = DataInterfacePtr->DeltaX;
		DataInterfacePtr->DeltaX = 0;	//This will stop a possible X motion when the mouse is lifted off the pad
		DeltaY = DataInterfacePtr->DeltaY;
		DataInterfacePtr->DeltaY = 0;	//This will stop a possible Y motion when the mouse is lifted off the pad
		ArmCmdSend(/*DeltaX, DeltaY, Button*/);
		for(ix = 0; ix < 1000000; ix++); 	// A delay to allow time for a command to be executed
		SynthOS_sleep();
	}
}
#endif

/*
 * This taks handles the gripper
 */
void taskGripCtrl() {
	for(;;) {
		Button = DataInterfacePtr->Button;
		// Buttons handling
		if(Button == MWBUTTON_L)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, GRIPPER_CLOSE);
		else if(Button == MWBUTTON_R)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, GRIPPER_OPEN);
		else
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, PAUSE);
		SynthOS_sleep();
	}
}

/*
 * This taks handles the base
 */
void taskBaseCtrl() {
	int ix;
	for(;;) {
		DeltaX = DataInterfacePtr->DeltaX;
		DataInterfacePtr->DeltaX = 0;	//This will stop a possible X motion when the mouse is lifted off the pad
		// Base movement handling
		if(DeltaX > XThreshold)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_CW);
		else if(DeltaX < -XThreshold)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_CCW);
		else
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, BASE_PAUSE);
		for(ix = 0; ix < 250000; ix++); 	// A delay to allow time for a command to be executed
		SynthOS_sleep();
	}
}

/*
 * This taks handles the shoulder
 */
void taskShoulderCtrl() {
	int ix;
	for(;;) {
		DeltaY = DataInterfacePtr->DeltaY;
		DataInterfacePtr->DeltaY = 0;	//This will stop a possible Y motion when the mouse is lifted off the pad
		// Shoulder movement handling
		if(DeltaY > YThreshold)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_UP);
		else if(DeltaY < -YThreshold)
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_DOWN);
		else
			XUartLite_SendByte(XPAR_RS232_P160_BASEADDR, SHOULDER_PAUSE);
		for(ix = 0; ix < 3000000; ix++); 	// A delay to allow time for a command to be executed
		SynthOS_sleep();
	}
}



/*=====This Used to be pcmouse.c. Because of SynthOS limitations it has to be combind=====*/


#define MAX_BYTES	128		/* number of bytes for buffer */
#define	SCALE		3		/* default scaling factor for acceleration */
#define	THRESH		5		/* default threshhold for acceleration */
#define MAX_TRIES	1000000	/* number of retries to read the input fifo */

/* states for the mouse*/
#define	IDLE			0		/* start of byte sequence */
#define	XSET			1		/* setting x delta */
#define	YSET			2		/* setting y delta */
#define	XADD			3		/* adjusting x delta */
#define	YADD			4		/* adjusting y delta */

/* values in the bytes returned by the mouse for the buttons*/
#define MS_LEFT_BUTTON		2
#define MS_RIGHT_BUTTON		1
#define MS_MIDDLE_BUTTON	0


/* Bit fields in the bytes sent by the mouse.*/
#define TOP_FIVE_BITS		0xf8
#define BOTTOM_THREE_BITS	0x07
#define TOP_BIT			0x80
#define SIXTH_BIT		0x40
#define BOTTOM_TWO_BITS		0x03
#define THIRD_FOURTH_BITS	0x0c
#define BOTTOM_SIX_BITS  	0x3f

static int		state;		/* IDLE, XSET, ... */
static int		buttons;	/* current mouse buttons pressed*/
static int		availbuttons;	/* which buttons are available */
static long		xd;		/* change in x */
static long		yd;		/* change in y */

static int		left;		/* because the button values change */
static int		middle;		/* between mice, the buttons are */
static int		right;		/* redefined */

static unsigned char	*bp;		/* buffer pointer */
static int		nbytes;		/* number of bytes left */
static unsigned char	buffer[MAX_BYTES];	/* data bytes read */


//Cyg_ErrNo error;		/* error output*/
//cyg_io_handle_t handle;		/* mouse device handle */
//cyg_serial_info_t info;		/* serial port config info */
//cyg_uint32 len;				/* data length */

/* This funtion does not belong here but with the Xilinx BSP instead */
//static int UARTRead(char *BuffP, Xuint32 *nBytes);

static void MOU_TripletRead (char *BuffP, Xuint32 *nBytes);

static int  	ParseMS(int);		/* routine to interpret MS mouse */

/*
 * Open up the mouse device.
 */
int MOU_Open(void)
{
	
	/* microsoft mouse*/
	left = MS_LEFT_BUTTON;
	right = MS_RIGHT_BUTTON;
	middle = MS_MIDDLE_BUTTON;


	/* initialize data*/
	availbuttons = left | middle | right;
	state = IDLE;
	nbytes = 0;
	buttons = 0;
	xd = 0;
	yd = 0;

	return 0;
}

/*
 * Close the mouse device.
 */
void MOU_Close(void)
{

}

/*
 * Get mouse buttons supported
 */
int MOU_GetButtonInfo(void)
{
	return availbuttons;
}

/*
 * Get default mouse acceleration settings
 */
void MOU_GetDefaultAccel(int *pscale,int *pthresh)
{
	*pscale = SCALE;
	*pthresh = THRESH;
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
int MOU_Read(long *dx, long *dy, long *dz, int *bptr)
{
	int Btn;
	Xuint32 nbytes;

	/*
	 * If there are no more bytes left, then read some more,
	 * waiting for them to arrive.  On a signal or a non-blocking
	 * error, return saying there is no new state available yet.
	 */
		bp = buffer;	// ???
		//UARTRead(bp, &nbytes);	//This functions appears to create race conditions
		MOU_TripletRead(bp, &nbytes);
		if (nbytes != 3) {
			return nbytes;
		}

	/*
	 * Loop over all the bytes read in the buffer, parsing them.
	 * When a complete state has been read, return the results,
	 * leaving further bytes in the buffer for later calls.
	 */
	while (nbytes-- > 0) {
		if (ParseMS((int) *bp++)) {
			*dx = xd;
			*dy = yd;
			*dz = 0;
			Btn = 0;
			if(buttons & MS_LEFT_BUTTON)
				Btn |= MWBUTTON_L;
			if(buttons & MS_RIGHT_BUTTON)
				Btn |= MWBUTTON_R;
			if(buttons & MS_MIDDLE_BUTTON)
				Btn |= MWBUTTON_M;
			*bptr = Btn;
			return 1;
		}
	}
	return 0;
}

/*
 * Input routine for Microsoft mouse.
 * Returns nonzero when a new mouse state has been completed.
 */
int ParseMS(int byte)
{
	switch (state) {
		case IDLE:
			if (byte & SIXTH_BIT) {
				buttons = (byte >> 4) & BOTTOM_TWO_BITS;
				yd = ((byte & THIRD_FOURTH_BITS) << 4);
				xd = ((byte & BOTTOM_TWO_BITS) << 6);
				state = XADD;
			}
			break;

		case XADD:
			xd |= (byte & BOTTOM_SIX_BITS);
			state = YADD;
			break;

		case YADD:
			yd |= (byte & BOTTOM_SIX_BITS);
			state = IDLE;
			if (xd > 127)
				xd -= 256;
			if (yd > 127)
				yd -= 256;
			return 1;
	}
	return 0;
}

#if 0
/*
* Logically this function belong with Xilinx BSP. It had to be added because
* Xilinx provide only a blocking version for reading input bytes.
* The function tries to read a byte from the input fifo (16 bytes deep),
* if its empty the function return 0 if not empty it tries to read three
* bytes from the UART and then return.
* Return value of 0 means the queue is was empty, Value of 1 or 2 means partial
* reading and the results should be ingnored. Return value of 3 indicate a good
* reading.
* This is a none blocking function. If the fifo is empty it returns immidiately.
* None blocking anables polling mode.
*/
Xuint8 MouseByte;	// For testing only
static int UARTRead(char *BuffP, Xuint32 *nBytes) {

	int ix;
	int BytesRead = 0;

	for(ix = 0; ix < MAX_TRIES; ix++) {	// Do some retries
		if(!XUartLite_mIsReceiveEmpty(XPAR_RS232_BASEADDR)) {
			*BuffP = (Xuint8)XIo_In32(XPAR_RS232_BASEADDR + XUL_RX_FIFO_OFFSET);
			MouseByte = *BuffP;
			BuffP++;
			BytesRead++;
			if(BytesRead >= 3)	// Should never be > 3
				break;
		}
	}
	*nBytes = BytesRead;
	return BytesRead;
}

#endif

/*
* Read a triplet bytes sent from a Microsoft serial mouse and return.
* When reading the function sync on the first byte 
* of the Microsoft serial mouse protocol and then read 
* the 2 following bytes
*/
static void MOU_TripletRead (char *BuffP, Xuint32 *nBytes) {

	Xuint32 BytesRead =0;
	Xuint8 InByte;
	
	while(1) {
		InByte = SynthOS_XUartLite_RecvByte(XPAR_RS232_BASEADDR);	//None blocking function - if no input it calls SynthOS_wait

		if(InByte >= 64) {	//First byte of a message triplet
			BuffP[0] = InByte;
			BytesRead = 1;
		}
		else if(BytesRead == 1 || BytesRead == 2) {
			BuffP[BytesRead] = InByte;
			BytesRead++;
			if(BytesRead >= 3) {
				*nBytes = BytesRead;
				break;
			}
		}
	}
}


/****************************************************************************/
/**
*
* This functions receives a single byte using the UART. The original function
* was blocking in that it was waiting for the receiver to become none empty
* before it reads from the receiver register.
* To support SynthOS operation this function modified to work in a none blocking
* mode. If the receiver has no input the function calls SynthOS wait to allow
* other tasks to run
*
* @param    BaseAddress is the base address of the device
*
* @return
*
* The byte of data received.
*
* @note
*
* None.
*
******************************************************************************/
Xuint8 SynthOS_XUartLite_RecvByte(Xuint32 BaseAddress)
{
    while (XUartLite_mIsReceiveEmpty(BaseAddress));
	//A SynthOS wait call should be placed here

    return (Xuint8)XIo_In32(BaseAddress + XUL_RX_FIFO_OFFSET);
}
