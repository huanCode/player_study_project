#pragma once
#ifndef _IAUDIOLAY_H
#define _IAUDIOLAY_H
#include "amcomdef.h"


class IAudioDecode
{
public:
	virtual MBool AudioDecode(MPChar buffer, MInt32& bufferSize,MInt64* out_pts) = 0;
	virtual MVoid SetAudioPts(MInt64 pts) = 0;
};


class IAudioPlay
{
public:
	virtual MBool Open() = 0;
	virtual MVoid Close() = 0;
	virtual MVoid Pause() = 0;
	virtual MVoid ReStart() = 0;
	virtual MBool Start() = 0;
	virtual MVoid Stop() = 0;
	MVoid SetInterface(IAudioDecode* object)
	{
		m_interfaceObject = object;
	};
	//virtual MBool Display(MPChar pBuffer,MInt32 bufferSize) = 0;

protected:
	IAudioDecode* m_interfaceObject;
};



#endif // !_IAUDIOLAY_H