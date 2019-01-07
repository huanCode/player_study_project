#include "stdafx.h"
#include "AudioScale.h"
#include "AudioPlayAAC.h"
AudioScale::AudioScale()
{
	m_pFrame = MNull;
	au_convert_ctx = MNull;
	//audioFile.Open("audioScale.pcm", mv3File::stream_write);
	m_count = 0;
	m_pcmBufferSize = 0;
}

MBool AudioScale::isEqual(AudioInfo& a, AudioInfo& b)
{
	if (a.channels == b.channels
		&& a.nb_samples == b.nb_samples
		&& a.sample_fmt == b.sample_fmt
		&& a.sample_rate == b.sample_rate)
	{
		return MTrue;
	}

	return MFalse;
}

MVoid AudioScale::Close()
{
	if (au_convert_ctx)
	{
		swr_free(&au_convert_ctx);
		au_convert_ctx = MNull;
	}

	if (m_pFrame)
	{
		av_frame_free(&m_pFrame);
		m_pFrame = MNull;
	}

}


MBool AudioScale::Open()
{

	if (au_convert_ctx)
	{
		return MTrue;
	}

	if (!m_pFrame)
	{
		m_pFrame = av_frame_alloc();
		if (!m_pFrame)
		{
			return MFalse;
		}
		int ret = av_samples_alloc(m_pFrame->data, m_pFrame->linesize,
									m_out_audio.channels,
									m_out_audio.nb_samples,
									(AVSampleFormat)m_out_audio.sample_fmt, 1);
		//m_pFrame->data[0] = (uint8_t *)av_malloc(192000 * 2);
		if (ret > 0)
		{
			m_pFrame->nb_samples = m_out_audio.nb_samples;
			m_pFrame->channels = m_out_audio.channels;
			m_pFrame->sample_rate = m_out_audio.sample_rate;
			m_pFrame->format = m_out_audio.sample_fmt;

			m_pcmBufferSize = m_out_audio.nb_samples * m_out_audio.channels * av_get_bytes_per_sample((AVSampleFormat)m_out_audio.sample_fmt);

		}
		else
		{
			av_frame_free(&m_pFrame);
			m_pFrame = MNull;
			return MFalse;
		}


	}

	//out_buffer = (uint8_t *)av_malloc(192000 * 2);


	MInt32	in_channels = m_in_audio.channels;
	int64_t in_channel_layout = av_get_default_channel_layout(in_channels);
	AVSampleFormat in_sample_fmt = (AVSampleFormat)m_in_audio.sample_fmt;
	int64_t in_sample_rate = m_in_audio.sample_rate;

	uint64_t out_channel_layout = av_get_default_channel_layout(m_out_audio.channels);
	uint64_t out_sample_rate = m_out_audio.sample_rate;



	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
		out_channel_layout, (AVSampleFormat)m_out_audio.sample_fmt, out_sample_rate,
		in_channel_layout, in_sample_fmt, in_sample_rate,
		0, NULL);

	if (!au_convert_ctx)
	{

		return MFalse;
	}

	int ret = swr_init(au_convert_ctx);
	if (ret != MERR_NONE)
	{

		return MFalse;
	}


	//au_convert_ctx = swr_alloc();
	//if (au_convert_ctx)
	//{
	//	int src_channel_layout = av_get_default_channel_layout(m_in_audio.channels);
	//	av_opt_set_int(au_convert_ctx, "in_channel_layout", src_channel_layout, 0);
	//	av_opt_set_int(au_convert_ctx, "in_sample_rate", m_in_audio.sample_rate, 0);
	//	av_opt_set_sample_fmt(au_convert_ctx, "in_sample_fmt", (AVSampleFormat)m_in_audio.sample_fmt, 0);

	//	//-----------dst
	//	int dst_channel_layout = av_get_default_channel_layout(m_out_audio.channels);
	//	av_opt_set_int(au_convert_ctx, "out_channel_layout", dst_channel_layout, 0);
	//	av_opt_set_int(au_convert_ctx, "out_sample_rate", m_out_audio.sample_rate, 0);
	//	av_opt_set_sample_fmt(au_convert_ctx, "out_sample_fmt", (AVSampleFormat)m_out_audio.sample_fmt, 0);

	//	if (swr_init(au_convert_ctx) >= 0)
	//	{
	//		return MTrue;
	//	}
	//}


	//if (au_convert_ctx)
	//{
	//	swr_free(&au_convert_ctx);
	//	au_convert_ctx = MNull;
	//}
	return MTrue;

}

MVoid AudioScale::SetInAudioIndo(AudioInfo	audioInfo)
{
	m_in_audio.channels = audioInfo.channels;
	m_in_audio.nb_samples = audioInfo.nb_samples;
	m_in_audio.sample_fmt = audioInfo.sample_fmt;
	m_in_audio.sample_rate = audioInfo.sample_rate;
}
MVoid AudioScale::SetOutAudioIndo(AudioInfo	audioInfo)
{
	m_out_audio.channels = audioInfo.channels;
	m_out_audio.nb_samples = audioInfo.nb_samples;
	m_out_audio.sample_fmt = audioInfo.sample_fmt;
	m_out_audio.sample_rate = audioInfo.sample_rate;
}


AVFrame* AudioScale::Scale(AVFrame *in_pFrame)
{
	if (!au_convert_ctx || !in_pFrame)
	{
		return MNull;
	}

	//MInt32 nb_sample = swr_get_delay(au_convert_ctx, m_in_audio.sample_rate) + in_pFrame->nb_samples;
	//int dst_nb_samples = av_rescale_rnd(nb_sample, m_out_audio.sample_rate, m_in_audio.sample_rate, AV_ROUND_UP);

	int size = 9000;
	int ret = swr_convert(au_convert_ctx, &m_pFrame->data[0], size, (const uint8_t **)in_pFrame->data, in_pFrame->nb_samples);
	if (ret < 0)
	{
		return MNull;
	}
	//m_pFrame->nb_samples = ret;
	av_samples_get_buffer_size(&m_pFrame->linesize[0], m_out_audio.channels, ret, (AVSampleFormat)m_out_audio.sample_fmt, 1);
	int count = 300;
	//if (m_count < count)
	//{
	//	audioFile.Write((MByte*)m_pFrame->data[0], m_pFrame->linesize[0]);
	//	
	//}
	//else if(m_count == count)
	//{
	//	audioFile.Close();
	//}
	m_count++;
	m_pFrame->linesize[0] = m_pFrame->linesize[0];
	m_pFrame->pts = in_pFrame->pts;

	return m_pFrame;
}

