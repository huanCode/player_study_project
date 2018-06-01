#include "stdafx.h"
#include "AudioScale.h"
#include "AudioPlayAAC.h"
AudioScale::AudioScale()
{
	m_pFrame = MNull;
	au_convert_ctx = MNull;
	audioFile.Open("audioScale.pcm", mv3File::stream_write);
	m_count = 0;
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
		int ret = av_samples_alloc(m_pFrame->data, NULL,
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

		}
		else
		{
			av_frame_free(&m_pFrame);
			m_pFrame = MNull;
			return MFalse;
		}


	}

	//out_buffer = (uint8_t *)av_malloc(192000 * 2);

	au_convert_ctx = swr_alloc();
	if (au_convert_ctx)
	{
		int src_channel_layout = av_get_default_channel_layout(m_in_audio.channels);
		av_opt_set_int(au_convert_ctx, "in_channel_layout", src_channel_layout, 0);
		av_opt_set_int(au_convert_ctx, "in_sample_rate", m_in_audio.sample_rate, 0);
		av_opt_set_sample_fmt(au_convert_ctx, "in_sample_fmt", (AVSampleFormat)m_in_audio.sample_fmt, 0);

		//-----------dst
		int dst_channel_layout = av_get_default_channel_layout(m_out_audio.channels);
		av_opt_set_int(au_convert_ctx, "out_channel_layout", dst_channel_layout, 0);
		av_opt_set_int(au_convert_ctx, "out_sample_rate", m_out_audio.sample_rate, 0);
		av_opt_set_sample_fmt(au_convert_ctx, "out_sample_fmt", (AVSampleFormat)m_out_audio.sample_fmt, 0);

		if (swr_init(au_convert_ctx) >= 0)
		{
			return MTrue;
		}
	}


	if (au_convert_ctx)
	{
		swr_free(&au_convert_ctx);
		au_convert_ctx = MNull;
	}
	return MFalse;

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

	MInt32 nb_sample = swr_get_delay(au_convert_ctx, m_in_audio.sample_rate) + in_pFrame->nb_samples;
	int dst_nb_samples = av_rescale_rnd(nb_sample, m_out_audio.sample_rate, m_in_audio.sample_rate, AV_ROUND_UP);
	int ret = swr_convert(au_convert_ctx, &m_pFrame->data[0], dst_nb_samples, (const uint8_t **)in_pFrame->data, in_pFrame->nb_samples);
	if (ret < 0)
	{
		return MNull;
	}
	m_pFrame->nb_samples = ret;
	av_samples_get_buffer_size(&m_pFrame->linesize[0], m_out_audio.channels, ret, (AVSampleFormat)m_out_audio.sample_fmt, 1);

	if (m_count < 300)
	{
		audioFile.Write((MByte*)m_pFrame->data[0], 4096);
		
	}
	else if(m_count == 300)
	{
		audioFile.Close();
	}
	m_count++;


	return m_pFrame;
}

