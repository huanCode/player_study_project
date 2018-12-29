#ifndef _DECODECMGR_H
#define _DECODECMGR_H
#include "IDecode.h"
class DecodecVideoMgr
{
public:
	DecodecVideoMgr();
	MBool Open(MInt32 iDecodeType);
	MVoid Close();
	Frame*	DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize, MInt64 pts, MInt64 dts);
private:



private:
	IDecode*	m_decode;
};

#endif // !_DECODECVIDEOMGR_H
