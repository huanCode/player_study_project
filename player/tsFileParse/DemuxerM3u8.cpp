#include "stdafx.h"
#include "DemuxerM3u8.h"
#include "mv3File.h"
#include "ToolString.h"
#include "amstring.h"
#include "ammem.h"
#include "DemuxerTs.h"
#include "AllConfig.h"
#define MAX_URL_SIZE 1024
#define AV_TIME_BASE            1000000



void DemuxerM3u8::Playlist::AddSegment(segment* ts)
{
	segmentList.AddNode(ts);
}


DemuxerM3u8::DemuxerM3u8()
{
	m_curIndex = 0;
	m_pTs = MNull;
	m_curSementIndex = 1;
	m_curPlaylist = MNull;

	m_beginDts = 0;
	m_beginPts = 0;
	m_bFirst = MTrue;

	m_duration = 0;
}

MBool DemuxerM3u8::ReadHeader(MPChar strUrl)
{
	//MPChar strUrl = "bipbopall.m3u8";

	//解析m3u8
	MDWord timeDuration = 0;
	MDWord timeBegin = MGetCurTimeStamp();
	if (!ParseM3u8(strUrl))
	{
		return MFalse;
	}
	timeDuration = MGetCurTimeStamp() - timeBegin;
	printf("DemuxerM3u8::ReadHeader 0 time = %d ms\r\n", timeDuration);
	//判断上面解析的m3u8是否为嵌套,是嵌套则去解析新的

	if (m_playlistList.GetSize() > 1 || m_playlistList.GetHeadNode()->segmentList.GetSize() == 0)
	{
		
		m_curPlaylist = m_playlistList.GetHeadNode();
		if (!ParseM3u8(m_curPlaylist->strUrl, m_curPlaylist))
		{
			return MFalse;
		}
	}
	else
	{
		m_curPlaylist = m_playlistList.GetHeadNode();
	}
	m_bTsReady = MTrue;


	return switchSegment();
}

MBool DemuxerM3u8::switchSegment()
{
	if (m_pTs)
	{
		m_pTs->Close();
		delete m_pTs;
		m_pTs = MNull;
	}


	if (m_curPlaylist->segmentList.GetSize() >= m_curSementIndex)
	{
		//m_curSementIndex = 70;
		//printf("tsIndex = %d  mtl = %s\r\n", m_curSementIndex, m_curPlaylist->segmentList.GetNodePtrByIndex(m_curSementIndex)->url);
		if (m_dataRead->Open(m_curPlaylist->segmentList.GetNodePtrByIndex(m_curSementIndex)->url))
		{
			MPChar tmpBuffer = (MPChar)MMemAlloc(MNull, PROBE_BUFFER_SIZE);
			if (tmpBuffer == MNull)
			{
				return MFalse;
			}

			MMemSet(tmpBuffer, 0, PROBE_BUFFER_SIZE);
			MInt32 iReadSize = 0;
			m_dataRead->Read(&tmpBuffer, PROBE_BUFFER_SIZE, iReadSize);
			if (iReadSize != PROBE_BUFFER_SIZE)
			{
				return MFalse;
			}

			m_pTs = DemuxerTs::read_probe(tmpBuffer, PROBE_BUFFER_SIZE);
			if (m_pTs == MNull)
			{
				return MFalse;
			}
			m_dataRead->Close();
			m_pTs->SetDataRead(m_dataRead);
			m_pTs->ReadHeader(m_curPlaylist->segmentList.GetNodePtrByIndex(m_curSementIndex)->url);
			m_curSementIndex++;
			return MTrue;
		}

	}

	return MFalse;
}

MBool DemuxerM3u8::ReadPacket(AVPkt** pkt)
{
	MBool ret = MFalse;
	if (m_pTs)
	{

		while((ret = m_pTs->ReadPacket(pkt)) && !(*pkt)->isGetPacket)
		{
			//表示这个切片已经读完了
			
			if (!switchSegment())
			{
				return MFalse;
			}
		}

		if (ret)
		{
			if (m_bFirst)
			{
				m_bFirst = MFalse;
				m_beginDts = (*pkt)->dts;
				m_beginPts = (*pkt)->pts;
			}

			(*pkt)->dts = (((*pkt)->dts - m_beginDts) * 1000) / 90000;
			(*pkt)->pts = (((*pkt)->pts - m_beginPts) * 1000) / 90000;
			if ((*pkt)->mediaType == AV_MEDIA_TYPE_VIDEO)
			{
				printf("video pts = %lld \r\n", (*pkt)->pts);
			}
			else if((*pkt)->mediaType == AV_MEDIA_TYPE_AUDIO)
			{
				printf("audio pts = %lld \r\n", (*pkt)->pts);
			}
			
		}
		else
		{
			int i = 1;
		}

		
	}
	return ret;
}

MVoid DemuxerM3u8::Close()
{

}


MBool   DemuxerM3u8::Seek(MInt64 seekTimeStamp)
{
	//seekTimeStamp = seekTimeStamp / 1000;
	MInt32 size = m_curPlaylist->segmentList.GetSize();
	MInt32 duration = 0;
	for (MInt32 i = 1; i <= size; i++)
	{
		duration += m_curPlaylist->segmentList.GetNodePtrByIndex(i)->duration * 1000;
		if (duration >= seekTimeStamp)
		{
			m_curSementIndex = i;
			return switchSegment();
		}
	}

	return MTrue;
}


void DemuxerM3u8::ff_parse_key_val_cb(void* srcData, MPChar key, MInt32 keyLen, MPChar value)
{
	if (srcData == MNull || key == MNull || value == MNull)
	{
		return;
	}


	Playlist* playlist = (Playlist*)srcData;

	if (!MStrNCmp(key, "BANDWIDTH=", keyLen)) {
		playlist->iBandwidth = MAtoi(value);
	}
	//else if (!strncmp(key, "AUDIO=", key_len)) {
	//	*dest = info->audio;
	//	*dest_len = sizeof(info->audio);
	//}
	//else if (!strncmp(key, "VIDEO=", key_len)) {
	//	*dest = info->video;
	//	*dest_len = sizeof(info->video);
	//}
	//else if (!strncmp(key, "SUBTITLES=", key_len)) {
	//	*dest = info->subtitles;
	//	*dest_len = sizeof(info->subtitles);
	//}

}

IDemuxer* DemuxerM3u8::hls_probe(MPChar p_buffer, MUInt32 p_size)
{
	if (MStrNCmp(p_buffer, "#EXTM3U", 7) == 0) {
		return new DemuxerM3u8();
	}
		

	return MNull;
}





MBool DemuxerM3u8::ParseM3u8(MPChar strUrl, Playlist* playlist)
{
	if (m_dataRead == MNull)
	{
		return MFalse;
	}

	MInt32 max_buffer_size = 1024;
	MPChar bufferBegin = (MPChar)MMemAlloc(MNull, max_buffer_size);
	if (!bufferBegin)
	{
		return MFalse;
	}
	MPChar bufferEnd = bufferBegin + max_buffer_size;
	
	MMemSet(bufferBegin, 0, max_buffer_size);

	MChar line[MAX_URL_SIZE] = { 0 };
	MChar tmp_strUrl[MAX_URL_SIZE] = {0};

	MInt32 iCopySize = 0;
	MInt32 iReadByte = 0;

	

	MPChar ptr = MNull;
	MInt32 lineSize = 0;

	//is_variant = 1,表示为嵌套m3u8
	//is_segment = 1,表示为ts url
	MInt32 is_segment = 0, is_variant = 0;


	MBool	isFirst = MTrue;

	MInt32	bufferSize = 0;

	MInt64 duration = 0;

	
	if (!m_dataRead->Open(strUrl))
	{
		MMemFree(MNull, bufferBegin);
		return MFalse;
	}

	MPChar strRealUrl = MNull;
	MVoid* tmp = strRealUrl;
	m_dataRead->GetConfig(GET_CFG_HTTP_LOCATION_URL, &tmp);
	strRealUrl = (MPChar)tmp;
	if (!strRealUrl)
	{
		strRealUrl = strUrl;
	}

	MBool ret = MFalse;
	MInt32 iBufferEmptySize = 0;
	MInt32 bufferReadSize = 0;

	MPChar bufferUsedBegin = bufferBegin;
	MPChar bufferUsedEnd = bufferBegin;
	MBool  bLoop = MTrue;
	while (bLoop)
	{


		
		MBool retRead = MFalse;
		do
		{
			iBufferEmptySize = bufferEnd - bufferUsedEnd;
			retRead = m_dataRead->Read(&bufferUsedEnd, iBufferEmptySize, bufferReadSize);
			if (!retRead)
			{
				goto Exit;

			}
			else if (bufferReadSize==0)
			{
				break;
			}

			bufferUsedEnd += bufferReadSize;
			iBufferEmptySize -= bufferReadSize;

		} while (bufferUsedEnd != bufferEnd);



		while (bufferUsedBegin<= bufferUsedEnd)
		{
			lineSize = ToolString::Read_line(bufferUsedBegin, bufferUsedEnd, line, MAX_URL_SIZE);
			if (lineSize == 0)
			{
				MInt32 size = bufferUsedEnd - bufferUsedBegin;
				MMemCpy(bufferBegin, bufferUsedBegin, size);
				bufferUsedBegin = bufferBegin;
				bufferUsedEnd = bufferBegin + size;
				break;
			}
			else
			{
				bufferUsedBegin += lineSize;
			}


			//iCopySize += lineSize;
			if (MStrCmp((MPChar)line, "#EXTM3U") && isFirst)
			{
				goto Exit;
				//return MFalse;
			}
			else if (ToolString::av_strstart(line, "#EXT-X-STREAM-INF:", &ptr)) {
				//说明目前该m3u8是顶级，里面的内容包含二级m3u8
				// #EXT-X-STREAM-INF:值指定码率值
				is_variant = 1;	//表示为嵌套m3u8
				playlist = new Playlist();
				ToolString::ff_parse_key_value(ptr, DemuxerM3u8::ff_parse_key_val_cb, playlist);

			}
			else if (ToolString::av_strstart(line, "#EXT-X-KEY:", &ptr)) {
				//暂时忽略
			}
			else if (ToolString::av_strstart(line, "#EXT-X-MEDIA:", &ptr)) {
				//暂时忽略
			}
			else if (ToolString::av_strstart(line, "#EXT-X-TARGETDURATION:", &ptr)) {
				//  #EXT-X-TARGETDURATION:指定最大的媒体段时间长度(秒), 
				//  #EXTINF:指定的时间长度必须小于或等于这个最大值。
				if (playlist == MNull)
				{
					playlist = createPlaylist(playlist);
					if (playlist == MNull)
					{
						//return MFalse;
						goto Exit;
					}
					m_playlistList.AddNode(playlist);
				}

			}
			else if (ToolString::av_strstart(line, "#EXT-X-MEDIA-SEQUENCE:", &ptr)) {
				//暂时忽略
				if (playlist == MNull)
				{
					playlist = createPlaylist(playlist);
					if (playlist == MNull)
					{
						//return MFalse;
						goto Exit;
					}
					m_playlistList.AddNode(playlist);
				}

			}
			else if (ToolString::av_strstart(line, "#EXT-X-PLAYLIST-TYPE:", &ptr)) {
				//暂时忽略
				if (playlist == MNull)
				{
					playlist = createPlaylist(playlist);
					if (playlist == MNull)
					{
						//return MFalse;
						goto Exit;
					}
					m_playlistList.AddNode(playlist);
				}
			}
			else if (ToolString::av_strstart(line, "#EXT-X-MAP:", &ptr)) {
				//暂时忽略
				if (playlist == MNull)
				{
					playlist = createPlaylist(playlist);
					if (playlist == MNull)
					{
						//return MFalse;
						goto Exit;
					}
					m_playlistList.AddNode(playlist);
				}
			}
			else if (ToolString::av_strstart(line, "#EXT-X-ENDLIST", &ptr)) {
				bLoop = MFalse;
				break;
			}
			else if (ToolString::av_strstart(line, "#EXTINF:", &ptr)) {
				is_segment = 1;
				duration = MStoi64(ptr) /** AV_TIME_BASE*/;
			}
			else if (ToolString::av_strstart(line, "#EXT-X-BYTERANGE:", &ptr)) {
				//暂时忽略
			}
			else if (ToolString::av_strstart(line, "#", NULL)) {
				isFirst = MFalse;
				continue;

			}
			else if (line[0]) {

				if (is_variant)
				{
					MInt32 strNameSize = sizeof(line) + sizeof(strRealUrl) + 1;
					playlist->strUrl = (MPChar)MMemAlloc(MNull, strNameSize);
					ToolString::ff_make_absolute_url(playlist->strUrl, strNameSize, "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8", line);
					//MMemCpy(playlist->strName, line, sizeof(line));
					m_playlistList.AddNode(playlist);
					is_variant = 0;
				}

				if (is_segment)
				{
					if (playlist == MNull)
					{
						playlist = createPlaylist(playlist);
						if (playlist == MNull)
						{
							goto Exit;
							//return MFalse;
						}
						m_playlistList.AddNode(playlist);
					}


					segment *seg = (segment *)MMemAlloc(MNull, sizeof(struct segment));
					if (seg == MNull)
					{
						goto Exit;
						//return MFalse;
					}

					seg->duration = duration;
					m_duration += duration;
					ToolString::ff_make_absolute_url(tmp_strUrl, sizeof(tmp_strUrl), strRealUrl, line);

					seg->url = ToolString::av_strdup(tmp_strUrl);
					if (!seg->url) {
						MMemFree(MNull, seg);
						goto Exit;
						//return MFalse;
					}
					//static int j = 0;
					//if (j == 160)
					//{
					//	int a = 1;
					//}
					//j++;

					is_segment = 0;
					playlist->segmentList.AddNode(seg);
				}



			}

			isFirst = MFalse;
		}
	}

	ret = MTrue;
Exit:	
	MMemFree(MNull, bufferBegin);
	m_dataRead->Close();

	return ret;
}