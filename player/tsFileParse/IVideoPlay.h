#pragma once
#ifndef _IVIDEOPLAY_H
#define _IVIDEOPLAY_H
#include "amcomdef.h"

class IVideoPlay
{
public:
	virtual MBool Open() = 0;
	virtual MVoid Close() = 0;
	virtual MBool Display(MPChar pBuffer) = 0;


};



#endif // !_IVIDEOPLAY_H