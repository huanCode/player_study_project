// tsFileParse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mv3File.h"
#include "TsStream.h"
#include "ParseHls.h"
#include "ToolList.h"
#include "ToolString.h"
#include "SourceParse.h"


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
	//ToolList<int*>  ll;
	//int* i1 = new int();
	//int* i2 = new int();
	//int* i3 = new int();
	//*i1 = 1;
	//*i2 = 2;
	//*i3 = 3;
	//ll.AddNode(i1);
	//ll.AddNode(i2);
	//ll.AddNode(i3);
	//int* ii = ll.GetNodePtrByIndex(4);





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

	avcodec_register_all();
	AVCodecID codec_id = AV_CODEC_ID_H264;
	AVCodec *pCodec = avcodec_find_decoder(codec_id);
	if (!pCodec) {
		printf("Codec not found\n");
		return -1;
	}


	AVCodecContext *pCodecCtx = NULL; 
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx) {
		printf("Could not allocate video codec context\n");
		return -1;
	}

	AVCodecParserContext *pCodecParserCtx = NULL;
	pCodecParserCtx = av_parser_init(codec_id);
	if (!pCodecParserCtx) {
		printf("Could not allocate video parser context\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec\n");
		return -1;
	}


	AVFrame	*pFrame;
	pFrame = av_frame_alloc();

	AVPacket packet;
	av_init_packet(&packet);


	//packet.data = (uint8_t *)pkt->bufferPkt;
	//packet.size = pkt->bufferPktSize;
	//int ret, got_picture;
	//ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);

	//if (ret < 0) {
	//	printf("Decode Error.\n");
	//	return ret;
	//}
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


		if (pkt)
		{
			delete pkt;
		}
	}



    return 0;
}

