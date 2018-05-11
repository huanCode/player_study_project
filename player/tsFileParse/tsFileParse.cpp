// tsFileParse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mv3File.h"
#include "TsStream.h"
#include "ParseHls.h"
#include "ToolList.h"
#include "ToolString.h"
#include "SourceParse.h"
#include "DecodeAAC.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};


inline MUInt16 to_UInt16(MByte* p)
{
	MUInt16 n = ((MByte*)p)[0];
	n <<= 8; 
	n += ((MByte*)p)[1]; return n;
}
inline MUInt8 to_UInt8(MByte* p)
{
	return *((MByte*)p);
}

int main()
{

	char* strUrl = "http://172.17.228.76/tms.content?url=udp://127.0.0.1:1111";
	strUrl = "http://38uni317.vod2.hongshiyun.net/target/hls/2017/01/13/660_3deb5b1a9a3f4f3d9c49f52f41a19a60_10_1280x720.m3u8";
	SourceParse sourse;
	sourse.Open(strUrl);	//嵌套
	AVPkt* pkt = MNull;
	MInt32 i = 0;
	//while (1)
	//{
	//	if (i == 307)
	//	{
	//		int a = 1;
	//	}
	//	sourse.ReadFrame(&pkt);
	//	i++;
	//	if (pkt)
	//	{
	//		delete pkt;
	//	}
	//	
	//}

	//sourse.ReadFrame(&pkt);
	MInt32 nb_samples = 1024;
	nb_samples = FFALIGN(nb_samples, 32);
	//MInt32 tt = av_rescale_rnd(1024,48000,44100,AV_ROUND_UP);
	DecodeAAC aac;
	if (!aac.Open())
	{
		return -1;
	}
	int ret = 0;
	while (1)
	{
		if (i == 307)
		{
			int a = 1;
		}
		sourse.ReadFrame(&pkt);

		if (pkt->mediaType == AV_MEDIA_TYPE_VIDEO)
		{

		}
		else if (pkt->mediaType == AV_MEDIA_TYPE_AUDIO)
		{
			aac.DecodeFrame(pkt->bufferPkt,pkt->bufferPktSize);
		}


		if (pkt)
		{
			delete pkt;
		}
	}



    return 0;
}

