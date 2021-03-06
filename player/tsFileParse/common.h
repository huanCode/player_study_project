#ifndef _COMMON_H
#define _COMMON_H
#include "AVConfig.h"
#include "ammem.h"
#include "amkernel.h"
MUInt16 to_UInt16(MPChar p);
MUInt8 to_UInt8(MPChar p);




#define RETURN_FALSE(ret) if(!ret){return MFalse;}

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


typedef struct _videoInfo
{
	_videoInfo()
	{
		width = 0;
		height = 0;
	}
	MUInt32	width;
	MUInt32	height;
}VideoInfo;


typedef struct _AVPKT
{
	_AVPKT()
	{
		Init();
	}

	~_AVPKT()
	{
		if (bufferPkt)
		{
			MMemFree(MNull, bufferPkt);
		}
		
	}

	MBool CopyBuffer(MPChar pBuffer, MInt32 bufferSize, AV_MediaType& type, AV_CodecID codeID,MInt64 iPts,MInt64 iDts,MBool iFlags)
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
		pts = iPts;
		dts = iDts;
		codeType = codeID;
		flags = iFlags;

	}

	MVoid Copy(_AVPKT* pkt)
	{
		pkt->bIsSync = bIsSync;
		pkt->bufferPkt = bufferPkt;
		pkt->bufferPktSize = bufferPktSize;
		pkt->mediaType = mediaType;
		pkt->isGetPacket = isGetPacket;
		pkt->pts = pts;
		pkt->dts = dts;
		pkt->codeType = codeType;
		pkt->flags = flags;
	}


	MVoid DepthCopy(_AVPKT* pkt)
	{
		pkt->bIsSync = bIsSync;

		pkt->CopyBuffer(bufferPkt, bufferPktSize, mediaType, codeType, pts,dts,flags);



	}

	MVoid Init()
	{
		bufferPkt = MNull;
		bufferPktSize = 0;
		bIsSync = 0;
		mediaType = AV_MEDIA_TYPE_UNKNOWN;
		codeType = _AV_CODEC_ID_NONE;
		isGetPacket = MFalse;
		pts = -1;
		dts = -1;
	}

	MPChar			bufferPkt;
	MInt32			bufferPktSize;
	MBool			bIsSync;			//only for video frame,True is key frame
	AV_MediaType	mediaType;	//audio/video
	AV_CodecID		codeType;
	MBool			isGetPacket;	//false:表示ts切片已经读完,这个是通过读数据设置
	MInt64			pts;
	MInt64			dts;
	MBool			flags;	//true:key frame

}AVPkt;

typedef struct _Frame
{
	MPChar	pBuffer;
	MInt32  iBufferSize;
	MInt64  pts;
	MBool	bSuccess;

	MVoid Init()
	{
		pBuffer = MNull;
		iBufferSize = 0;
		pts = 0;
		bSuccess = MFalse;
	}

}Frame;


class PlayLock
{
public:
	PlayLock()
	{
		m_lock = MNull;
	}
	~PlayLock()
	{
		UnLock();
	}
	MBool Init()
	{
		m_lock = MMutexCreate();
		if (m_lock)
		{
			return MTrue;
		}
		return MFalse;
	}

	MVoid Lock()
	{
		if (m_lock)
		{
			MMutexLock(m_lock);
		}
		
	}

	MVoid UnLock()
	{
		if (m_lock)
		{
			MMutexUnlock(m_lock);
		}
	}

	MVoid UnInit()
	{
		if (m_lock)
		{
			MMutexDestroy(m_lock);
		}
	}

private:
	MHandle					m_lock;

};

#endif // !_COMMON_H
