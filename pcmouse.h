#ifndef _PCMOUSE_H_
#define _PCMOUSE_H_

/* Driver interface */
int  	MOU_Open();
void 	MOU_Close(void);
int  	MOU_GetButtonInfo(void);
void		MOU_GetDefaultAccel(int *pscale,int *pthresh);
int  	MOU_Read(long *dx, long *dy, long *dz, int *bptr);

/* Mouse button bits*/
#define MWBUTTON_L	04
#define MWBUTTON_M	02
#define MWBUTTON_R 01

//The x,y threshold intend to dampen the arm movement and reduce jitter
#define XThreshold 10	//Any mouse movement greater then this will cause an arm movement
#define YThreshold 10	//Any mouse movement greater then this will cause an arm movement

#endif
