#include "amcomdef.h"
inline MUInt16 to_UInt16(MPChar p);
inline MUInt8 to_UInt8(MPChar p);

MUInt8 get8(MChar*& p);
MUInt16 get16(MChar*& p);

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct _avPacketInfo
{
	MUInt32	dwFrameSize;
	MUInt32	dwTimeStart;
	MUInt32	dwTimeSpan;
	MUInt32 dwExtraInfoSize;    //the size of extra info in front of the media data 
	MBool	bIsSync;			//only for video frame
}AVPacketInfo;
