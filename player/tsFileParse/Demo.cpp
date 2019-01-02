// tsFileParse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mv3File.h"
#include "TsStream.h"
#include "ParseHls.h"
#include "ToolList.h"
#include "ToolString.h"
#include "SourceParse.h"
#include "AudioPlayAAC.h"
#include "DecodeAAC.h"
#include "DecodeH264.h"
#include "Player.h"
//#include "amc_aac_encoder.h"
//#include "SoundTouch.h"
//#include "WavFile.h"
//using namespace soundtouch;
extern "C"
{
#include "libavcodec/avcodec.h"
};
#include <windows.h>

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

#define BUFF_SIZE 4096


//void aacEndecodeSoundTouch()
//{
//
//
//	SoundTouch soundTouch;
//
//	
//
//	return;
//
//}


int main(int      argc, char    *argv[])
{


	Player player;
	player.Start("http://video.ewt360.com:8080/hls-vod/ewt360/files/20140704mp4/maoxianqin02.f4v.m3u8");
	Sleep(200000);

	return 0;




	char* strUrl = "http://hlsglsb.wasu.tv/1480682957527_561859.m3u8?action=hls&Contentid=CP23010020161201084109";
	strUrl = "http://nf6jwj7r.vod2.danghongyun.com/target/hls/2017/09/06/777_e9cac11a41a24c93b56d276270e910d1_17_1280x720.m3u8";
	SourceParse sourse;
	sourse.Open(strUrl);	//嵌套
	AVPkt* pkt = MNull;
	MInt32 i = 0;

	AVPacket pkt_tmp;
	AVCodecContext *m_pCodecCtx;
	avcodec_register_all();
	AVCodecID codec_id = AV_CODEC_ID_H264;
	AVCodec *pCodec = avcodec_find_decoder(codec_id);
	if (!pCodec) {
		return 0;
	}

	m_pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!m_pCodecCtx) {

		return 0;
	}

	if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0) {
		return 0;
	}

	av_init_packet(&pkt_tmp);

	int ret = 0;
	while (1)
	{
		if (i == 307)
		{
			int a = 1;
		}
		sourse.ReadFrame(&pkt);

		if (pkt->mediaType == 0)
		{
			pkt_tmp.data = (uint8_t*)pkt->bufferPkt;
			pkt_tmp.size = pkt->bufferPktSize;
			ret = avcodec_send_packet(m_pCodecCtx, &pkt_tmp);

		}


		i++;
		if (pkt)
		{
			delete pkt;
		}
		
	}

	//sourse.ReadFrame(&pkt);
	//MInt32 nb_samples = 1024;
	//nb_samples = FFALIGN(nb_samples, 32);
	////MInt32 tt = av_rescale_rnd(1024,48000,44100,AV_ROUND_UP);
	//DecodeAAC aac;

	//DecodeH264 video;

	//AudioPlayAAC audioPlay;
	//audioPlay.Open();
	//video.Open();
	//if (!aac.Open())
	//{
	//	return -1;
	//}
	//int ret = 0;

	//Frame* frame = MNull;
	//while (1)
	//{
	//	if (i == 307)
	//	{
	//		int a = 1;
	//	}
	//	sourse.ReadFrame(&pkt);

	//	if (pkt->mediaType == AV_MEDIA_TYPE_VIDEO)
	//	{
	//		frame = video.DecodeFrame(pkt->bufferPkt, pkt->bufferPktSize, pkt->pts, pkt->dts);
	//	}
	//	else if (pkt->mediaType == AV_MEDIA_TYPE_AUDIO)
	//	{
	//		//frame = aac.DecodeFrame(pkt->bufferPkt,pkt->bufferPktSize, pkt->pts, pkt->dts);
	//		//audioPlay.Display((MPChar)frame->pBuffer, frame->iBufferSize);
	//	}


	//}



    return 0;
}

