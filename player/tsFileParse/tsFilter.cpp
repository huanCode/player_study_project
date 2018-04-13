#include "stdafx.h"
#include "tsFilter.h"
#include "common.h"
#include "TsStreamDef.h"
#include "TsStream.h"
#include "ammem.h"

#include<assert.h>
#define SECTION_HEADER_SIZE_8_BYTE		8



tsSection::tsSection()
{
	m_section_index = 0;
	m_section_length = 0;
	m_last_ver = -1;
	m_crc = 0;
	m_last_crc = 0;
	m_section_buf = new MChar[MAX_SECTION_SIZE];
	memset(m_section_buf,0, MAX_SECTION_SIZE);
}

MVoid tsSection::write_section_data(TsStream* p_tsStream, const MPChar p_buf, MUInt32 p_buf_size, MBool p_is_start)
{
	if (!m_section_buf)
	{
		return;
	}


	if (p_is_start) {
		//˵��section��СС��payload
		memcpy(m_section_buf, p_buf, p_buf_size);
		m_section_index = p_buf_size;
		m_section_length = -1;
		m_end_of_section_reached = MFalse;

		/* compute section length if possible */
		if (m_section_length == -1 && m_section_index >= 3) {
			MUInt32 section_length = (AV_RB16(m_section_buf + 1) & 0xfff) + 3;	//����section_length
			if (section_length > MAX_SECTION_SIZE)
				return;
			m_section_length = section_length;
		}
	}
	else {
		if (m_end_of_section_reached)
			return;
		MUInt32 remainder_len = MAX_SECTION_SIZE - m_section_index;
		if (p_buf_size < remainder_len)
		{
			memcpy(m_section_buf + m_section_index, p_buf, p_buf_size);
			m_section_index += p_buf_size;
		}

	}

	if (m_section_length != -1 && m_section_index >= m_section_length)
	{
		/*˵���Ѿ�ȫ����ϳ�*/ 
		MBool crc_valid = MFalse;
		m_end_of_section_reached = MTrue;
		/*ʡ��crc��֤*/

		parse(p_tsStream, m_section_buf, m_section_length);
		
	}
}



MBool tsFilter::skip_identical(MInt32 iVersion, MInt32 iCrc)
{
	if (m_last_version == iVersion /*&& tssf->last_crc == tssf->crc*/)
		return MTrue;

	m_last_version = iVersion;

	return 0;
}

MInt32 tsFilter::parse_section_header(MPChar buffer_section_header, SectionHeader &section_header)
{
	int val;

	val = get8(buffer_section_header);	//0 byte

	if (val < 0)
		return -1;

	section_header.tid = val;

	section_header.section_length = get16(buffer_section_header) & 0xFFF;

	val = get16(buffer_section_header);	//3 byte
	if (val < 0)
		return val;

	section_header.id = val;

	val = get8(buffer_section_header);	//5 byte
	if (val < 0)
		return val;
	section_header.version = (val >> 1) & 0x1f;
	val = get8(buffer_section_header);	//6 byte
	if (val < 0)
		return val;
	section_header.sec_num = val;
	val = get8(buffer_section_header);	//7 byte
	if (val < 0)
		return val;
	section_header.last_sec_num = val;
	return 0;
}
/*========================================================================*/

MUInt32 tsSectionPat::parse(TsStream* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size)
{
	SectionHeader section_header;
	MPChar pat_data = p_buffer;
	MInt32 ret = parse_section_header(pat_data, section_header);
	if (ret != 0)
	{
		return -1;
	}

	if (skip_identical(section_header.version))
	{
		return 0;
	}

	pat_data += SECTION_HEADER_SIZE_8_BYTE;

	if (section_header.tid != PAT_TID)
		return 0;

	MInt32 sid = 0;
	MInt32 pmt_pid = 0;

	//10�ֽ�,������transport_stream_id��last_section_number����CRC32
	MUInt16 program_length = (section_header.section_length - 9) / 4;
	for (MUInt32 i = 0; i < program_length; i++)
	{
		sid = get16(pat_data);
		if (sid < 0)
			break;

		if (sid == 0)
		{
			//sid == 0,��ʾ����NIT����������
			continue;
		}
		//sid��Ŀ��Ϊ0x0001ʱ, ��ʾ����PMT��PID = 0x1000����4096

		pmt_pid = get16(pat_data);
		if (pmt_pid < 0)
		{
			break;
		}
		pmt_pid &= 0x1fff;
		//if (pmt_pid != PMT_PID)
		//{
		//	return -1;
		//}

		tsFilter* filter = p_tsStream->add_filter(TYPE_PMT,pmt_pid);

		//Ŀǰֻ֧��һ��pmt,so break
		break;
	}
	return 0;
}

/*=========================================================================*/

MBool tsSectionPmt::is_pes_stream(int stream_type, MUInt32 prog_reg_desc)
{
	MInt32 y = AV_RL32("CUEI");
	return !(stream_type == 0x13 || (stream_type == 0x86 && prog_reg_desc == AV_RL32("CUEI")));
}


//MVoid tsSectionPmt::mpegts_find_stream_type(MInt32 stream_type, const StreamType *types)
//{
//	for (; types->stream_type; types++) {
//		if (stream_type == types->stream_type) {
//			if (st->codecpar->codec_type != types->codec_type ||
//				st->codecpar->codec_id != types->codec_id) {
//				st->codecpar->codec_type = types->codec_type;
//				st->codecpar->codec_id = types->codec_id;
//				st->internal->need_context_update = 1;
//			}
//			st->request_probe = 0;
//			return;
//		}
//	}
//}

MUInt32 tsSectionPmt::parse(TsStream* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size)
{
	SectionHeader section_header;
	MInt32 ret = parse_section_header(p_buffer, section_header);
	if (ret != 0)
	{
		return -1;
	}

	if (skip_identical(section_header.version))
	{
		return 0;
	}


	MPChar pData = p_buffer + SECTION_HEADER_SIZE_8_BYTE;
	MPChar pData_end = p_buffer + p_buffer_size - 4;
	if (section_header.tid != PMT_TID)
		return 0;


	m_pcr_pid = get16(pData);
	if (m_pcr_pid < 0)
		return 0;
	m_pcr_pid &= 0x1fff;

	MUInt16 program_info_length = get16(pData);
	//pData += 2;
	if (program_info_length < 0)
		return -1;
	program_info_length &= 0xfff;
	if (program_info_length >= 2)
	{
		return -1;
	}
	//����program_info����
	pData += program_info_length;


	MInt32 stream_type = 0;
	MInt32 pid = 0;

	p_tsStream->m_stopParse = 3;
	MInt32 iIndex = 0;
	while (true)
	{
		//27 0x1b H264
		stream_type = get8(pData);
		pid = get16(pData) & 0x1fff;
		if (pid < 0)
		{
			return -1;
		}
		tsFilter* filter = p_tsStream->get_filter(pid);
		if (filter == MNull && is_pes_stream(stream_type))
		{
			filter = p_tsStream->add_filter(TYPE_PES,pid);
			if (filter == MNull)
			{
				return -1;
			}
			tsSectionPes* filterPes = (tsSectionPes*)filter;
			if (!filterPes->mpegts_find_stream_type(stream_type, ISO_types))
			{
				return -1;
			}
			filterPes->SetIndex(iIndex++);
			
		}
		else
		{
			return -1;
		}

		MUInt16  desc_list_len = get16(pData);
		if (desc_list_len < 0)
			return -1;
		desc_list_len &= 0xfff;
		pData += desc_list_len;
		if (pData_end < pData)
		{
			return -1;
		}
		else if (pData_end == pData)
		{
			return 0;
		}



		//15 0xFΪAAC
		//m_streamAudio.stream_type = section_start_pos[17];
		//m_streamAudio.pid = to_UInt16(section_start_pos + 18) & 0x1fff;
		//desc_list_len = to_UInt16(section_start_pos + 20);
		//if (desc_list_len < 0)
		//	return -1;
		//desc_list_len &= 0xfff;
	}
}


/*==========================================================================*/

MBool tsSectionPes::mpegts_find_stream_type(MInt32 stream_type, const StreamType *types)
{
	for (; types->stream_type; types++) {
		if (stream_type == types->stream_type) {
			if (m_mediaType != types->codec_type || m_mediaCodecID != types->codec_id) {
				m_mediaType = types->codec_type;
				m_mediaCodecID = types->codec_id;
				m_stream_type = stream_type;
			}
			return MTrue;
		}
	}

	return MFalse;
}

MUInt32 tsSectionPes::parse(TsStream* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size)
{
	//is_start = 1����ʾ���ʱpes�Ŀ�ʼ
	//is_start = 0,��ʾ���ʱpes�еĺ����ֽ�
	MBool start = MFalse;
	MBool b = p_tsStream->m_isStart;
	if (p_tsStream->m_isStart)
	{
		if (m_mediaType == AV_MEDIA_TYPE_VIDEO)
		{
			int i = 0;
		}

		//��ʾ��һ֡��Ƶ�Ѿ���ȡ���
		if (m_state == MPEGTS_PAYLOAD && m_buffer_size > 0)
		{
			if (!p_tsStream->m_avpkt.CopyBuffer(m_buffer, m_buffer_size))
			{

				return -1;
			}
			p_tsStream->m_stopParse = 1;

		}

		m_pts = -1;
		m_dts = -1;
		m_buffer_size = 0;
		m_pes_header_size = 0;
		m_total_size = 0;
		m_header_size = 0;
		MMemSet(m_header, 0,MAX_PES_HEADER_SIZE);
		m_state = MPEGTS_HEADER;

	}

	MUInt16 length = 0;
	MInt32	code = 0;
	MPChar pBuffer = p_buffer;
	MInt32 tBuf_size = p_buffer_size;
	while (tBuf_size > 0)
	{
		if (m_state == MPEGTS_HEADER)
		{
			length = PES_START_SIZE - m_header_size;
			if (length > tBuf_size)
				length = tBuf_size;

			memcpy(m_header + m_header_size, pBuffer, length);
			m_header_size += length;
			tBuf_size -= length;
			pBuffer += length;
			if (m_header_size == PES_START_SIZE)
			{
				//-----------------------------------------------------------------------------
				//| packet_start_code_prefix 3byte | stream_id 1byte | PES_packet_length 2byte
				//-------------------------------------------------------------------------------
				//packet_start_code_prefix : 0x00 00 01
				//stream_id	:

				/* we got all the PES or section header. We can now
				* decide */
				if (m_header[0] == 0x00 && m_header[1] == 0x00 && m_header[2] == 0x01)
				{
					/* it must be an MPEG-2 PES stream */
					code = m_header[3] | 0x100;
					m_stream_id = m_header[3];
					if (code == 0x1be)
					{
						goto skip;
					}
					//video�Ĵ�Сһ���Ƿ���0,��ʾ��Сδ֪����ôֻ���Զ��������ֵ
					//audio�����ܻ�ȡ��С��
					m_total_size = AV_RB16(m_header + 4);	//get pes size
					if (m_total_size == 0)
					{
						//m_total_size  ,˵��pes�Ĵ�С��δ֪��
						m_total_size = MAX_PES_PAYLOAD;
					}
					
					if (m_total_size == MAX_PES_PAYLOAD)
					{
						//��Ƶ������
						if (m_buffer == MNull)
						{
							m_buffer = new MChar[m_total_size + AV_INPUT_BUFFER_PADDING_SIZE];
							if (!m_buffer)
							{
								return -1;
							}
						}
						memset(m_buffer, 0, m_total_size + AV_INPUT_BUFFER_PADDING_SIZE);
					}
					else
					{
						//audio����ʽ
						if (m_buffer)
						{
							delete m_buffer;
							m_buffer = MNull;
						}
						m_buffer = new MChar[m_total_size + AV_INPUT_BUFFER_PADDING_SIZE];
						if (!m_buffer)
						{
							return -1;
						}
						memset(m_buffer, 0, m_total_size + AV_INPUT_BUFFER_PADDING_SIZE);
					}
	

					if (code != 0x1bc && code != 0x1bf && /* program_stream_map, private_stream_2 */
						code != 0x1f0 && code != 0x1f1 && /* ECM, EMM */
						code != 0x1ff && code != 0x1f2 && /* program_stream_directory, DSMCC_stream */
						code != 0x1f8) {                  /* ITU-T Rec. H.222.1 type E stream */
						m_state = MPEGTS_PESHEADER;
	
					}
					else {
						m_pes_header_size = 6;
						m_state = MPEGTS_PAYLOAD;
						m_header_size = 0;
					}
				}
				else
				{
skip:
					m_state = MPEGTS_SKIP;
					continue;
				}

			}
	


		}
		else if (m_state == MPEGTS_PESHEADER)
		{
			length = PES_HEADER_SIZE - m_header_size;
			if (length < 0)
			{
				return -1;
			}

			if (length > tBuf_size)
				length = tBuf_size;

			memcpy(m_header + m_header_size, pBuffer, length);
			m_header_size += length;
			pBuffer += length;
			tBuf_size -= length;
			if (m_header_size == PES_HEADER_SIZE) {
				m_pes_header_size = m_header[8] + 9;
				m_state = MPEGTS_PESHEADER_FILL;
			}
			//break;

		}
		else if (m_state == MPEGTS_PESHEADER_FILL)
		{

			length = m_pes_header_size - m_header_size;
			if (length < 0)
			{
				return -1;
			}

			if (length > tBuf_size)
			{
				length = tBuf_size;
			}
			memcpy(m_header + m_header_size, pBuffer, length);
			
			m_header_size += length;
			pBuffer += length;
			tBuf_size -= length;

			if (m_header_size == m_pes_header_size)
			{
				
				MUInt32 pts_dts_flags = m_header[7];
				MUInt8* pes_head_data = m_header + m_header_size;
				if ((pts_dts_flags & 0xc0) == 0x80) {
					m_dts = m_pts = ff_parse_pes_pts(pes_head_data);
					pes_head_data += 5;
				}
				else if ((pts_dts_flags & 0xc0) == 0xc0) {
					m_pts = ff_parse_pes_pts(pes_head_data);
					pes_head_data += 5;
					m_dts = ff_parse_pes_pts(pes_head_data);
					pes_head_data += 5;
				}
				if (pts_dts_flags & 0x01)
				{
					//��չ����
					return -1;
				}
				m_state = MPEGTS_PAYLOAD;
				m_header_size = 0;
				//m_buffer_size = 0;
				if (m_stream_type == 0x12 && p_buffer_size > 0) {
					return -1;
				}
				if (m_stream_type == 0x15 && p_buffer_size >= 5) {
					/* skip metadata access unit header */
					return -1;
				}


			}

		}
		else if (m_state == MPEGTS_PAYLOAD)
		{
			if (m_buffer)
			{
				MInt32 copySize = 0;
				if (m_header_size > 0 && p_buffer_size > m_total_size)
				{
					return -1;
				}
				else if (m_header_size == 0 && tBuf_size > m_total_size)
				{
					//������Ƶ���ݿ��ܾ���С�ڻ�ʣ���buffer��С,�������0xff���
					copySize = m_total_size;
				}
				else
				{
					copySize = tBuf_size;
				}


				//��Ҫ����Ƶ֡���ݣ�ͨ����ЩСbufferƴ������
				memcpy(m_buffer + m_buffer_size, pBuffer, copySize);
				m_buffer_size += copySize;



				if (!p_tsStream->m_stopParse && m_total_size < MAX_PES_PAYLOAD && m_pes_header_size + tBuf_size == m_total_size + PES_START_SIZE)
				{
					p_tsStream->m_stopParse = 2;
					if (!p_tsStream->m_avpkt.CopyBuffer(m_buffer, m_buffer_size))
					{

						return -1;
					}

				}
			}



			p_buffer_size = 0;

			//p_tsStream->m_stopParse = MTrue;
			break;
		}
		else if (m_state == MPEGTS_SKIP)
		{
			p_buffer_size = 0;
			break;
		}


	}





	return 1;
}


inline MInt64 tsSectionPes::ff_parse_pes_pts(const MUInt8 *buf) {
	return (MInt64)(*buf & 0x0e) << 29 |
		(AV_RB16(buf + 1) >> 1) << 15 |
		AV_RB16(buf + 3) >> 1;
}



tsFilter* FilterFactory::CreateFilter(MInt32 type,MInt32 pid)
{
	tsFilter* filter = MNull;
	switch (type)
	{
	case TYPE_SDT:
		filter = new tsSectionSdt();
		break;
	case TYPE_PAT:
		filter = new tsSectionPat();
		break;
	case TYPE_PMT:
		filter = new tsSectionPmt();
		break;
	case TYPE_PES:
		filter = new tsSectionPes();
		break;
	default:
		break;
	}
	if (filter)
	{
		filter->SetPid(pid);
	}

	return filter;
}
