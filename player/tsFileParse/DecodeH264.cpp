#include "stdafx.h"
#include "DecodeH264.h"
#include "common.h"
#include "mv3File.h"

//AVERROR_BSF_NOT_FOUND = -1179861752
//AVERROR_BUG = -558323010
//AVERROR_DECODER_NOT_FOUND = -1128613112
//AVERROR_DEMUXER_NOT_FOUND = -1296385272
//AVERROR_ENCODER_NOT_FOUND = -1129203192
//AVERROR_EOF = -541478725
//AVERROR_EXIT = -1414092869
//AVERROR_FILTER_NOT_FOUND = -1279870712
//AVERROR_INVALIDDATA = -1094995529
//AVERROR_MUXER_NOT_FOUND = -1481985528
//AVERROR_OPTION_NOT_FOUND = -1414549496
//AVERROR_PATCHWELCOME = -1163346256
//AVERROR_PROTOCOL_NOT_FOUND = -1330794744
//AVERROR_STREAM_NOT_FOUND = -1381258232
//AVERROR_BUG2 = -541545794
//AVERROR_UNKNOWN = -1313558101




DecodeH264::DecodeH264()
{

	m_pCodecCtx = MNull;
	m_pFrame = MNull;
	av_init_packet(&m_packet);

	m_img_convert_ctx = MNull;

	m_bSwsInit = MTrue;
	m_out_buffer = MNull;

	m_pFrameYUV = MNull;

	m_yuv420pSize = 0;
}


MBool	DecodeH264::Open()
{

	Close();


	m_pFrame = av_frame_alloc();
	if (!m_pFrame)
	{
		return MFalse;
	}

	avcodec_register_all();
	AVCodecID codec_id = AV_CODEC_ID_H264;
	AVCodec *pCodec = avcodec_find_decoder(codec_id);
	if (!pCodec) {
		return MFalse;
	}

	m_pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!m_pCodecCtx) {

		return MFalse;
	}

	if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0) {
		return MFalse;
	}

	av_init_packet(&m_packet);
	return MTrue;
}


MVoid	DecodeH264::Close()
{
	if (m_pCodecCtx)
	{
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);
		m_pCodecCtx = MNull;
	}

	if (m_img_convert_ctx)
	{
		sws_freeContext(m_img_convert_ctx);
		m_img_convert_ctx = MNull;
	}

	if (m_pFrameYUV)
	{
		av_frame_free(&m_pFrameYUV);
	}

	if (m_pFrame)
	{
		av_frame_free(&m_pFrame);
	}

	if (m_out_buffer)
	{
		MMemFree(MNull, m_out_buffer);
		m_out_buffer = MNull;
	}
	

}

MVoid DecodeH264::SetInfo(MPVoid info)
{
	if (!info)
	{
		return;
	}
	VideoInfo* videoInfo = (VideoInfo*)info;
	m_videoInfo.width = videoInfo->width;
	m_videoInfo.height = videoInfo->height;
}

Frame*	DecodeH264::DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize,MInt64 pts, MInt64 dts)
{

	if (!m_pCodecCtx || !srcBuffer)
	{
		return MNull;
	}

	

	int ret = 0;
	while (1)
	{
		m_packet.data = (uint8_t *)srcBuffer;
		m_packet.size = srcBufferSize;
		m_packet.pts = pts;
		m_packet.dts = dts;
		ret = avcodec_send_packet(m_pCodecCtx, &m_packet);
		if (ret < 0) {
			return MNull;
		}

		if (m_bSwsInit)
		{
			if (!OpenSwsCtx())
			{
				return MNull;
			}
		}
		while (ret >= 0) {
			ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				m_pOneFrame.bSuccess = MFalse;
				return &m_pOneFrame;
;
			}
			else if (ret < 0) {
				return MNull;
			}

			if (ret >= 0) {
				//FILE *fp_out;
				//char* filepath_out = "test1280x720.yuv";
				//fp_out = fopen(filepath_out, "wb");
				//if (!fp_out) {
				//	printf("Could not open output YUV file\n");
				//	return -1;
				//}
				//int i = 0;
				//unsigned char* tempptr = NULL;
				//tempptr = pFrame->data[0];
				//for (i = 0; i<pFrame->height; i++) {
				//	fwrite(tempptr, 1, pFrame->width, fp_out);     //Y 
				//	tempptr += pFrame->linesize[0];
				//}
				//tempptr = pFrame->data[1];
				//for (i = 0; i<pFrame->height / 2; i++) {
				//	fwrite(tempptr, 1, pFrame->width / 2, fp_out);   //U
				//	tempptr += pFrame->linesize[1];
				//}
				//tempptr = pFrame->data[2];
				//for (i = 0; i<pFrame->height / 2; i++) {
				//	fwrite(tempptr, 1, pFrame->width / 2, fp_out);   //V
				//	tempptr += pFrame->linesize[2];
				//}

				//fclose(fp_out);


				sws_scale(m_img_convert_ctx, (const uint8_t* const*)m_pFrame->data, m_pFrame->linesize, 0, m_pCodecCtx->height, m_pFrameYUV->data, m_pFrameYUV->linesize);
				
				m_pOneFrame.pBuffer = (MPChar)m_pFrameYUV->data[0];
				m_pOneFrame.iBufferSize = m_pFrameYUV->linesize[0];
				m_pOneFrame.pts = m_pFrame->pts;
				m_pOneFrame.bSuccess = MTrue;

				mv3File		videoYuvFileRead;
				MBool ret = MFalse;
				if (videoYuvFileRead.Open("1280_720.yuv420", mv3File::stream_read))
				{
					//ret = videoYuvFileRead.Read((MByte*)yuvBuffer, yuv420Size, readSize);
					videoYuvFileRead.Close();
				}




				return &m_pOneFrame;
			}
		}
	}


	return MNull;
}

MVoid  DecodeH264::Flush_buffers()
{
	if (m_pCodecCtx)
	{
		avcodec_flush_buffers(m_pCodecCtx);
	}
}

MBool DecodeH264::OpenSwsCtx()
{

	if (!m_img_convert_ctx)
	{
		m_img_convert_ctx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt,
			m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
		RETURN_FALSE(m_img_convert_ctx)
	}

	m_yuv420pSize = avpicture_get_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);

	if (!m_pFrameYUV)
	{
		m_pFrameYUV = av_frame_alloc();
		RETURN_FALSE(m_pFrameYUV)
	}

	if (!m_out_buffer)
	{
		//av_image_get_buffer_size
		MInt32 bufferSize = avpicture_get_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);
		m_out_buffer = (uint8_t*)MMemAlloc(MNull, bufferSize);
		RETURN_FALSE(m_out_buffer)
		MMemSet(m_out_buffer, 0, bufferSize);
	}

	//
	avpicture_fill((AVPicture *)m_pFrameYUV, m_out_buffer, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);

	m_bSwsInit = MFalse;
	return MTrue;
}
