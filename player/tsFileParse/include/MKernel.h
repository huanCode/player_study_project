/*----------------------------------------------------------------------------------------------
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and 		
* confidential information. 
* 
* The information and code contained in this file is only for authorized ArcSoft employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify ArcSoft and 
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/
/*
 * MKernel.h
 *
 * In this file, define some simple functions for mutex, event and thread.
 *
 * History
 *    
 * --2004-08-02 Junming Lv
 * - init version 
 * 
 *
 */
	

#ifndef __MKENREL_H__
#define __MKENREL_H__

#ifndef MWAIT_INFINITE
#define MWAIT_INFINITE		(~0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//Interface for Mutex

MHandle		MMutexCreate();
MRESULT		MMutexDestroy(MHandle hMutex);
MRESULT		MMutexLock(MHandle hMutex);
MRESULT		MMutexUnlock(MHandle hMutex);
 

//Interface for Event
MHandle		MEventCreate(MBool bAutoReset);
MRESULT		MEventDestroy(MHandle hEvent);
MRESULT		MEventWait(MHandle hEvent, MDWord dwTimeOut);
MRESULT		MEventSignal(MHandle hEvent);
MRESULT		MEventReset(MHandle hEvent);

//Interface for Thread

typedef		MDWord	(*MThreadProc)(MPVoid lpPara); 

MHandle		MThreadCreate(MThreadProc proc, MPVoid lpPara);
MRESULT		MThreadDestory(MHandle hThread);

MRESULT		MThreadExit(MHandle hThread);


MRESULT		MThreadSleep(MHandle hThread, MDWord dwMilliseconds);
MRESULT		MThreadResume(MHandle hThread);
MRESULT		MThreadSuspend(MHandle hThread);

MRESULT		MThreadSetPriority(MHandle hThread, MLong lPriority);

#ifdef __cplusplus
}
#endif


#endif