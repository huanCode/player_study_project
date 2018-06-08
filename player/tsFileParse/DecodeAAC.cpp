#include "stdafx.h"
#include "DecodeAAC.h"



DecodeAAC::DecodeAAC()
{
	m_pCodecCtx = MNull;
	m_pFrame = MNull;
	av_init_packet(&m_packet);

	m_pAudioScale = MNull;

	m_out_audio.sample_rate = 44100;
	m_out_audio.channels = 2;
	m_out_audio.nb_samples = 1024;
	m_out_audio.sample_fmt = AV_SAMPLE_FMT_S16;
	//audioFile.Open("audioDecode.pcm", mv3File::stream_write);
	m_count = 0;
}

MBool	DecodeAAC::Open()
{

	Close();


	m_pFrame = av_frame_alloc();
	if (!m_pFrame)
	{
		return MFalse;
	}

	avcodec_register_all();
	AVCodecID codec_id = AV_CODEC_ID_AAC;
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


MVoid	DecodeAAC::Close()
{
	if (m_pCodecCtx)
	{
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);
	}

}

Frame*	DecodeAAC::DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize, MInt64 pts, MInt64 dts)
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
			return MFalse;
		}
		while (ret >= 0) {
			ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);

			//MInt32 si = av_get_bytes_per_sample(m_pCodecCtx->sample_fmt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				return MNull;
			}
			else if (ret < 0) {
				return MNull;
			}
			
			if (ret >= 0) {


				//if (m_count < 300)
				//{
				//	audioFile.Write((MByte*)m_pFrame->data, 8192);
				//	
				//}
				//else if(m_count == 300)
				//{
				//	audioFile.Close();
				//}
				//m_count++;

				AVFrame* frame = MNull;

				m_in_audio.channels = m_pCodecCtx->channels;
				m_in_audio.nb_samples = m_pCodecCtx->frame_size;
				m_in_audio.sample_fmt = m_pCodecCtx->sample_fmt;
				m_in_audio.sample_rate = m_pCodecCtx->sample_rate;

				if (!AudioScale::isEqual(m_in_audio, m_out_audio))
				{
					if (m_pAudioScale == MNull)
					{
						m_pAudioScale = new AudioScale();
						if (!m_pAudioScale)
						{
							return MNull;
						}

						m_pAudioScale->SetInAudioIndo(m_in_audio);
						m_pAudioScale->SetOutAudioIndo(m_out_audio);
						if (!m_pAudioScale->Open())
						{
							return MNull;
						}
						
					}

					frame = m_pAudioScale->Scale(m_pFrame);
				}
				else
				{
					frame = m_pFrame;
				}

				if (frame)
				{

					m_pOneFrame.pBuffer = (MPChar)frame->data[0];
					m_pOneFrame.iBufferSize = frame->linesize[0];
					m_pOneFrame.pts = frame->pts;
					m_pOneFrame.bSuccess = MTrue;
					return &m_pOneFrame;
				}

			}
		}
	}


	return MNull;
}