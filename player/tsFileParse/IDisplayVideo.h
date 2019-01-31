#pragma once
#ifndef _IVIDEOPLAY_H
#define _IVIDEOPLAY_H
#include "amcomdef.h"
#include "common.h"
class IDisplayVideo
{
public:
	virtual MBool Open() = 0;
	virtual MVoid Close() = 0;
	virtual MVoid Display(MPChar pBuffer) = 0;
	 MVoid SetVideoInfo(VideoInfo* info)
	{
		m_info.width = info->width;
		m_info.height = info->height;
	}
protected:
	VideoInfo	m_info;

};



#endif // !_IVIDEOPLAY_H