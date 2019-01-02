#include "stdafx.h"
#include "tsFilter.h"
#include "TsStream.h"
#include "common.h"
#include "ammem.h"

#define SECTION_HEADER_SIZE_8_BYTE		8


#define TS_PACKET_SYNC_BYTE		0x47




//#define NIT_TID	  0xXX	私用表，tid自定义

#define M4OD_TID  0x05
#define SDT_TID   0x42


#define TS_PACKET_HEADER_SIZE	4
#define PES_START_SIZE  6
#define PES_HEADER_SIZE 9


#define TS_FEC_PACKET_SIZE 204
#define TS_DVHS_PACKET_SIZE 192
#define TS_PACKET_SIZE 188

#define TS_MAX_PACKET_SIZE 204

//#define AV_RB32(x)  ((((const uint8_t*)(x))[0] << 24) | /
//(((const uint8_t*)(x))[1] << 16) | /
//(((const uint8_t*)(x))[2] << 8) | /
//((const uint8_t*)(x))[3])
//#define AV_RB32(x)  ((((const uint8_t*)(x))[0] << 24) | /
//(((const uint8_t*)(x))[1] << 16) | /
//(((const uint8_t*)(x))[2] << 8) | /
//((const uint8_t*)(x))[3])

TsStream::TsStream()
{
	m_pcr_pid = 0;
	m_pes_state = MPEGTS_HEADER;
	m_pes_header_size = 0;
	memset(&m_packet,0,sizeof(Packet));
	memset(&m_buffer, 0, 512000);
	//MBool ret = m_fileWrite.Open("bigbuckbunny_480x272.h265", mv3File::stream_write);
	m_isStart = MFalse;
	m_packetBuffer = MNull;
	m_trackNum = 0;

	m_stopParse = 0;

	m_width = 0;
	m_height = 0;

	m_audioCount = 0;

	m_hasVideo = MFalse;
	m_hasAudio = MFalse;

	m_bFirst = MTrue;
	m_beginDts = 0;
	m_beginPts = 0;


	for (MInt32 i = 0; i < FILTER_NUM; i++)
	{
		m_filter[i] = MNull;
	}
	//file.Open("tmp.h264", mv3File::stream_write);
	//audioFile.Open("audio.aac", mv3File::stream_write);
}


MInt32	TsStream::Packet_Size = 0;

MBool TsStream::Init()
{

	//MHandle m_hHttp = Http_Open("hhh", HTTP_POST, 0);
	tsFilter* pat = add_filter(TYPE_PAT,PAT_PID);
	tsFilter* sdt = add_filter(TYPE_SDT,SDT_PID);
	if (pat == MNull || sdt == MNull)
	{
		goto Exit;
	}

	m_packetBuffer = (MPChar)MMemAlloc(MNull, TsStream::Packet_Size);
	if (m_packetBuffer == MNull)
	{
		return MFalse;
	}
	MMemSet(m_packetBuffer, 0, PACKET_SIZE);


	return MTrue;
Exit:

	Release();

	return MFalse;
}


MInt32 TsStream::get_packet_size(MPChar buf, MInt32 size)
{
	int score, fec_score, dvhs_score;

	if (size < (TS_FEC_PACKET_SIZE * 5 + 1))
		return 0;

	score		= analyze(buf, size, TS_PACKET_SIZE, 0);
	dvhs_score	= analyze(buf, size, TS_DVHS_PACKET_SIZE, 0);
	fec_score	= analyze(buf, size, TS_FEC_PACKET_SIZE, 0);


	if (score > fec_score && score > dvhs_score)
		return TS_PACKET_SIZE;
	else if (dvhs_score > score && dvhs_score > fec_score)
		return TS_DVHS_PACKET_SIZE;
	else if (score < fec_score && dvhs_score < fec_score)
		return TS_FEC_PACKET_SIZE;
	else
		return 0;
}

MInt32 TsStream::analyze(MPChar buf, MInt32 size, MInt32 packet_size, MInt32 probe)
{
	int stat[TS_MAX_PACKET_SIZE];
	int stat_all = 0;
	int i;
	int best_score = 0;

	memset(stat, 0, packet_size * sizeof(*stat));

	for (i = 0; i < size - 3; i++) {
		if (buf[i] == 0x47) {
			int pid = AV_RB16(buf + 1) & 0x1FFF;
			int asc = buf[i + 3] & 0x30;
			if (!probe || pid == 0x1FFF || asc) {
				int x = i % packet_size;
				stat[x]++;
				stat_all++;
				if (stat[x] > best_score) {
					best_score = stat[x];
				}
			}
		}
	}

	return best_score - FFMAX(stat_all - 10 * best_score, 0) / 10;
}


MUInt32 TsStream::mpegts_read_header()
{
	//MUInt32 ret = 0;
	//mv3File file;

	//if (!file.Open("playlist-00001.ts", mv3File::stream_read))
	//{
	//	return -1;
	//}
	//MByte packet[PACKET_SIZE] = { 0 };
	//int i = 0;
	//int length = file.GetFileSize();

	//if (file.Read(packet, PROBE_BUFFER_SIZE))
	//{
	//	read_probe(packet, PROBE_BUFFER_SIZE);
	//}
	////file.SetFileBeginPos();
	//MInt32 iReadByteLen = 0;
	//while (file.Read(packet, 188, iReadByteLen))
	//{
	//	//printf("i = %d ,curpos = %d\n",i++,file.GetFileCurPos());

	//	read_header(packet, 188);

	//	//ret = parse_ts(packet);
	//	//if (ret != 0)
	//	//{
	//	//	return ret;
	//	//}
	//	if (i == 32397)
	//	{
	//		int a = 1;
	//		m_fileWrite.Close();
	//	}

	//	printf("\n");
	//}


	return 0;
}

MVoid TsStream::Release()
{
	for (MInt32 i = 0;i < FILTER_NUM;i++)
	{
		if (m_filter[i] == MNull)
		{
			break;
		}
		delete m_filter[i];
		m_filter[i] = MNull;
	}
}

IParse* TsStream::read_probe(MPChar p_buffer, MUInt32 p_size)
{
	if (p_buffer == MNull || p_size < PROBE_BUFFER_SIZE)
	{
		return MNull;
	}
	MUInt32 check_count = p_size / PACKET_SIZE;
	MUInt32	standard_count = 10;
	if (check_count < standard_count)
	{
		return MNull;
	}

	MUInt32 best_score = 0;
	MUInt32 pos = 0;
	for (MUInt32 i = 1;i<=check_count;i++)
	{
		if (p_buffer[pos]== TS_PACKET_SYNC_BYTE)
		{
			best_score++;
			pos = i * PACKET_SIZE;
		}
	}
	if (check_count == standard_count)
	{
		TsStream::Packet_Size = PACKET_SIZE;
		IParse* obj = new TsStream();

		return obj;
		//return Init();
	}






	return MNull;
}


//
//MInt32 TsStream::read_header(MPByte p_buffer_packet, MUInt32 p_size)
//{
//	if (p_buffer_packet == MNull || p_size != PACKET_SIZE)
//	{
//		return -1;
//	}
//	ts_packet_header tsHeader;
//	parse_ts_packet_header(p_buffer_packet, tsHeader);
//	MPByte	pData = p_buffer_packet + TS_PACKET_HEADER_SIZE;
//	if (tsHeader.sync_byte != TS_PACKET_SYNC_BYTE || tsHeader.transport_error)
//	{
//		return -1;
//	}
//
//	if (tsHeader.pid == 0x1FFF || tsHeader.has_payload == MFalse)
//	{
//		return 0;
//	}
//
//	MUInt16 adaptation_length = 0;
//	if (tsHeader.has_adaptation)
//	{
//		adaptation_length = get8(pData) + 1;
//		if (adaptation_length > 1)
//		{
//			bool is_discontinuity = get8(pData) & 0x80;
//			if (is_discontinuity)
//			{
//				int i = 0;
//			}
//			//printf("is_discontinuity = %d =====================\n", is_discontinuity);
//		}
//		//printf("has_adaptation ,adaptation_length = %d\n", adaptation_length);
//	}
//	//printf("continuity_counter = %d\n", tsHeader.continuity_counter);
//
//
//
//	pData += adaptation_length + 1;
//	MInt32 remainderSize = PACKET_SIZE - adaptation_length - 1 - TS_PACKET_HEADER_SIZE;
//
//	tsFilter* filter = get_filter(tsHeader.pid);
//	if (!filter)
//	{
//		filter = add_filter(tsHeader.pid);
//	}
//
//
//	if (filter->GetType() == tsFilter::MPEGTS_SECTION)
//	{
//		if (tsHeader.bStart_payload)
//		{
//			/* pointer field present */
//			MByte* point_field = p_buffer_packet + TS_PACKET_HEADER_SIZE + adaptation_length;
//			MUInt16 point_field_length = point_field[0];
//			//MUInt32 expected_cc = tsHeader.has_payload ? (filter->last_cc + 1) & 0x0f : filter->last_cc;
//			//if (expected_cc == tsHeader.continuity_counter && point_field_length)
//			//{
//			//	filter->write_section_data(pData, remainderSize, MFalse);
//			//}
//			if (point_field_length)
//			{
//				return -1;
//			}
//			tsSection* filterTmp = (tsSection*)filter;
//			filterTmp->write_section_data(this, point_field + 1, remainderSize, MTrue);
//		}
//		else
//		{
//			//
//			return -1;
//		}
//	}
//	else
//	{
//		m_isStart = tsHeader.bStart_payload;
//		filter->parse(this,pData, remainderSize);
//	}
//
//
//	return 0;
//}


//MBool TsStream::_AVPKT::CopyBuffer(MPChar pBuffer, MInt32 bufferSize, AV_MediaType& type)
//{
//	if (pBuffer == MNull || bufferSize <= 0)
//	{
//		return MFalse;
//	}
//
//
//	bufferPkt = (MPChar)MMemAlloc(MNull, bufferSize);
//	if (bufferPkt == MNull)
//	{
//		return MFalse;
//	}
//
//	MMemCpy(bufferPkt, pBuffer, bufferSize);
//	bufferPktSize = bufferSize;
//	mediaType = type;
//
//}



MBool TsStream::handle_packets(MInt32 nb_packets)
{

	m_stopParse = 0;
	MInt32 packet_num = 0;

	MInt32 iReadSize = 0;
	MBool ret = MFalse;
	MInt32 total = 0;
	for (;;) {
		packet_num++;
		if (nb_packets != 0 && packet_num >= nb_packets) {
			ret = MFalse;
			break;
		}
		if (m_stopParse > 0)
		{
			int temp = 0;
			if (m_stopParse == 1)
			{
				//video
				temp = 1;
			}

			if (m_stopParse == 2)
			{
				//audio
				temp = 1;
			}

			if (m_stopParse == 3)
			{
				//read hearder end
				temp = 1;
			}

			ret = MTrue;
			break;
		}

		if (packet_num == 13)
		{
			int a = 0;
		}

		ret = m_dataRead->Read(&m_packetBuffer,TsStream::Packet_Size, iReadSize);
		if (!ret)
		{
			if ( iReadSize == 0)
			{
				m_avpkt.isGetPacket = MFalse;
				return MTrue;
			}
			else if(TsStream::Packet_Size != iReadSize)
			{
				return MFalse;
				
			}
		}
			

		total += iReadSize;
		if (!handle_packet(m_packetBuffer))
			break;
	}

	return ret;
}

MBool TsStream::handle_packet(MPChar pBuffer)
{
	ts_packet_header tsHeader;
	parse_ts_packet_header(pBuffer, tsHeader);
	MPChar	pData = pBuffer + TS_PACKET_HEADER_SIZE;
	MPChar  pBufferEnd = pBuffer + Packet_Size;
	if (tsHeader.sync_byte != TS_PACKET_SYNC_BYTE || tsHeader.transport_error)
	{
		return MFalse;
	}

	if (tsHeader.pid == 0x1FFF || tsHeader.has_payload == MFalse)
	{
		return MTrue;
	}

	

	MUInt8 adaptation_length = 0;
	if (tsHeader.has_adaptation)
	{
		adaptation_length = pBuffer[4];
		adaptation_length++;//算上pBuffer[4]这个字节
		//adaptation_length = get8(pData);
		if (adaptation_length > 1)
		{
			//bool is_discontinuity = get8(pData) & 0x80;
			//if (is_discontinuity)
			//{
			//	int i = 0;
			//}
			//printf("is_discontinuity = %d =====================\n", is_discontinuity);
		}

		//pData += adaptation_length;
		//printf("has_adaptation ,adaptation_length = %d\n", adaptation_length);
	}

	

	//printf("continuity_counter = %d\n", tsHeader.continuity_counter);



	
	//MInt32 remainderSize = PACKET_SIZE - adaptation_length - 1 - TS_PACKET_HEADER_SIZE;

	tsFilter* filter = get_filter(tsHeader.pid);
	if (!filter)
	{
		return MFalse;
	}


	if (filter->GetType() == tsFilter::MPEGTS_SECTION)
	{
		if (tsHeader.bStart_payload)
		{
			/* pointer field present */
			MPChar point_field = pBuffer + TS_PACKET_HEADER_SIZE + adaptation_length;
			MUInt16 point_field_length = point_field[0];
			//MUInt32 expected_cc = tsHeader.has_payload ? (filter->last_cc + 1) & 0x0f : filter->last_cc;
			//if (expected_cc == tsHeader.continuity_counter && point_field_length)
			//{
			//	filter->write_section_data(pData, remainderSize, MFalse);
			//}
			if (point_field_length)
			{
				return MFalse;
			}
			tsSection* filterTmp = (tsSection*)filter;
			point_field++;
			MUInt32 len = (pBufferEnd - point_field);
			filterTmp->write_section_data(this, point_field, len, MTrue);
		}
		else
		{
			//
			return MFalse;
		}
	}
	else
	{
		m_isStart = tsHeader.bStart_payload;
		pData = pData + adaptation_length;

		MUInt32 len = (pBufferEnd - pData);
		filter->parse(this, pData, len);
	}

	return MTrue;
}



MBool TsStream::ReadHeader(MPChar strUrl)
{
	//1、通过解析ts前面，得到ts packet_size,这步在read_probe已经处理过

	
	if (!m_dataRead->Open(strUrl))
	{
		return MFalse;
	}
	
	if (!Init())
	{
		return MFalse;
	}
	//if (!m_dataRead->Read(&m_packetBuffer, TsStream::Packet_Size, m_iReadSize))
	//{
	//	return MFalse;
	//}
	

	//if (m_iReadSize != TsStream::Packet_Size)
	//{
	//	return MFalse;
	//}
	
	return handle_packets();
}



MBool	TsStream::ReadPacket(AVPkt** pkt)
{
	static int a = 1;
	if (!handle_packets()) {
		return MFalse;
	}

	if (m_stopParse)
	{
		MInt32 w = 0;
		MInt32 h = 0;
		if (m_avpkt.mediaType == AV_MEDIA_TYPE_VIDEO)
		{
			MDWord slicetype = 0;
			slicetype = H264Parse::GetSliceType((MByte*)m_avpkt.bufferPkt, m_avpkt.bufferPktSize);

			if (slicetype == AMC_H264_UTL_ERR_PARAM)
				return MFalse;

			if (slicetype == I_SLICE || slicetype == IDR_SLICE)
			{
				m_avpkt.bIsSync = MTrue;
			}

			H264Parse::GetWidthAndHeight((MByte*)m_avpkt.bufferPkt, m_avpkt.bufferPktSize, w, h);
			//file.Write((MByte*)m_avpkt.bufferPkt, m_avpkt.bufferPktSize);
			//
			//file.Close();



		}
		else if (m_avpkt.mediaType == AV_MEDIA_TYPE_AUDIO)
		{
			//if (m_audioCount < 400)
			//{
			//	//audioFile.Write((MByte*)m_avpkt.bufferPkt, m_avpkt.bufferPktSize);
			//}
			//
			//if (m_audioCount == 400)
			//{
			//	audioFile.Close();
			//}
			//m_audioCount++;
			//int a = 1;
		}
	}



	AVPkt* pktTmp = new AVPkt();
	m_avpkt.Copy(pktTmp);
	m_avpkt.Init();
	*pkt = pktTmp;

	return MTrue;
}



MVoid TsStream::Close()
{
	if (m_dataRead)
	{
		m_dataRead->Close();
		m_dataRead = MNull;
	}
}

MVoid TsStream::parse_ts_packet_header(MPChar buffer, ts_packet_header &tsHeader)
{
	tsHeader.sync_byte = buffer[0];

	MUInt16 ts_header_23byte = to_UInt16(buffer + 1);

	//‘1’表示在相关的传输包中至少有一个不可纠正的错误位。
	tsHeader.transport_error = ts_header_23byte & 0x8000;

	//为1时，表示当前TS包的有效载荷中包含PES或者PSI的起始位置；
	//在前4个字节之后会有一个调整字节，其的数值为后面调整字段的长度length。因此有效载荷开始的位置应再偏移1 + [length]个字节。
	tsHeader.bStart_payload = ts_header_23byte & 0x4000;

	tsHeader.pid = ts_header_23byte & 0x1fff;


	//下面主要是判断adaptation_field_control（适配域控制标志）
	//表示包头是否有调整字段或有效负载。
	//‘00’为ISO/IEC未来使用保留；
	//‘01’仅含有效载荷，无调整字段；
	//‘10’ 无有效载荷，仅含调整字段；
	//‘11’ 调整字段后为有效载荷，调整字段中的前一个字节表示调整字段的长度length，有效载荷开始的位置应再偏移[length]个字节。
	//空包应为‘10’。
	tsHeader.has_adaptation = buffer[3] & 0x20;	//
	tsHeader.has_payload	= buffer[3] & 0x10;
	tsHeader.continuity_counter = buffer[3] & 0xf;

}







tsFilter* TsStream::add_filter(MInt32 type,MInt32 pid)
{
	for (int i = 0;i<FILTER_NUM;i++)
	{
		if (m_filter[i] == MNull)
		{
			m_filter[i] = FilterFactory::CreateFilter(type,pid);
			return m_filter[i];
		}
	}

	return MNull;
}

tsFilter*	TsStream::get_filter(MInt32 p_pid)
{
	for (int i = 0; i < FILTER_NUM; i++)
	{
		if (m_filter[i] && m_filter[i]->GetPid() == p_pid)
		{
			return m_filter[i];
		}
	}


	return MNull;
}