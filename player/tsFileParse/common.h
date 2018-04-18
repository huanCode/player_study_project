#ifndef _COMMON_H
#define _COMMON_H
#include "TsStreamDef.h"
#include "ammem.h"
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
	AV_MediaType	mediaType;	//audio/video
}AVPacketInfo;



typedef struct _AVPKT
{
	_AVPKT()
	{
		Init();
	}

	~_AVPKT()
	{
		MMemFree(MNull, bufferPkt);
	}

	MBool CopyBuffer(MPChar pBuffer, MInt32 bufferSize, AV_MediaType& type)
	{
		if (pBuffer == MNull || bufferSize <= 0)
		{
			return MFalse;
		}


		bufferPkt = (MPChar)MMemAlloc(MNull, bufferSize);
		if (bufferPkt == MNull)
		{
			return MFalse;
		}

		MMemCpy(bufferPkt, pBuffer, bufferSize);
		bufferPktSize = bufferSize;
		mediaType = type;
		isGetPacket = MTrue;

	}

	MVoid Copy(_AVPKT* pkt)
	{
		pkt->bIsSync = bIsSync;
		pkt->bufferPkt = bufferPkt;
		pkt->bufferPktSize = bufferPktSize;
		pkt->mediaType = mediaType;
		pkt->isGetPacket = isGetPacket;
	}

	MVoid Init()
	{
		bufferPkt = MNull;
		bufferPktSize = 0;
		bIsSync = 0;
		mediaType = AV_MEDIA_TYPE_UNKNOWN;
		isGetPacket = MFalse;
	}

	MPChar			bufferPkt;
	MInt32			bufferPktSize;
	MBool			bIsSync;			//only for video frame
	AV_MediaType	mediaType;	//audio/video
	MBool			isGetPacket;
}AVPkt;


#endif // !_COMMON_H
