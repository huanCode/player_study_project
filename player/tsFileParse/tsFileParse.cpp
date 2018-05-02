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

	sourse.ReadFrame(&pkt);

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


	packet.data = (uint8_t *)pkt->bufferPkt;
	packet.size = pkt->bufferPktSize;
	int ret, got_picture;
	ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);

	if (ret < 0) {
		printf("Decode Error.\n");
		return ret;
	}
	//TsStream ts;
	//ts.mpegts_read_header();


	//ParseHls ps;
	//ps.Parse();

	int a = 1;
	return 0;
	//TsStream ts;
	//ts.mpegts_read_header();



	//mv3File file;
	//
	//if (!file.Open("76.ts", mv3File::stream_read))
	//{
	//	return -1;
	//}

	//MByte packet[188] = {0};
	//MByte packet2[188] = { 0 };
	////file.Read(packet,188);
	////file.Read(packet2, 188);
	////file.Read(packet2, 188);
	//if (packet[0] != 0x47)            // ts sync byte
	//	return -1;

	//MUInt16 ts_header_23byte = to_UInt16(packet+1);

	////‘1’表示在相关的传输包中至少有一个不可纠正的错误位。
	//bool transport_error				= ts_header_23byte & 0x8000;
	//if (transport_error)
	//{
	//	return -1;
	//}
	////为1时，表示当前TS包的有效载荷中包含PES或者PSI的起始位置；
	////在前4个字节之后会有一个调整字节，其的数值为后面调整字段的长度length。因此有效载荷开始的位置应再偏移1 + [length]个字节。
	//bool payload_unit_start_indicator	= ts_header_23byte & 0x4000;
	//MUInt16	pid = ts_header_23byte & 0x1fff;
	//MUInt8 ts_header_4byte = to_UInt8(packet + 3);
	////下面主要是判断adaptation_field_control（适配域控制标志）
	////表示包头是否有调整字段或有效负载。
	////‘00’为ISO/IEC未来使用保留；
	////‘01’仅含有效载荷，无调整字段；
	////‘10’ 无有效载荷，仅含调整字段；
	////‘11’ 调整字段后为有效载荷，调整字段中的前一个字节表示调整字段的长度length，有效载荷开始的位置应再偏移[length]个字节。
	////空包应为‘10’。
	//bool adaptation_field_exist		= ts_header_4byte & 0x20;	//
	//bool payload_data_exist			= ts_header_4byte & 0x10;


	////------------------------------------------
	//MUInt8 payload_data_length = to_UInt8(packet + 4);
    return 0;
}

