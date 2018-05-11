#pragma once
#ifndef _IAUDIOLAY_H
#define _IAUDIOLAY_H
#include "amcomdef.h"

class IAudioPlay
{
public:
	virtual MBool Open() = 0;
	virtual MVoid Close() = 0;
	virtual MBool Display(MPChar pBuffer) = 0;


};



#endif // !_IAUDIOLAY_H