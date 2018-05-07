#include "DecodeH264.h"

DecodeH264::DecodeH264()
{

	m_pCodecCtx = MNull;
	m_pFrame = MNull;
	av_init_packet(&m_packet);
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

	return MTrue;
}


MBool	DecodeH264::Close()
{
	if (m_pCodecCtx)
	{
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);
	}

}

MBool	DecodeH264::DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize)
{

	if (!m_pCodecCtx || !srcBuffer)
	{
		return MFalse;
	}

	int ret = 0;
	while (1)
	{
		m_packet.data = (uint8_t *)srcBuffer;
		m_packet.size = srcBufferSize;

		ret = avcodec_send_packet(m_pCodecCtx, &m_packet);
		if (ret < 0) {
			return MFalse;
		}

		while (ret >= 0) {
			ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				return MTrue;
			}
			else if (ret < 0) {
				return MFalse;
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

				return MFalse;
			}
		}
	}


	return MFalse;
}