#include "stdafx.h"
#include "ParseHls.h"
#include "mv3File.h"
#include "ToolString.h"
#include "amstring.h"
#include "ammem.h"
#include "TsStream.h"
#define MAX_URL_SIZE 4096
#define AV_TIME_BASE            1000000



void ParseHls::Playlist::AddSegment(segment* ts)
{
	segmentList.AddNode(ts);
}


ParseHls::ParseHls()
{
	m_curIndex = 0;
	m_pTs = MNull;
}

MBool ParseHls::ReadHeader(MPChar strUrl)
{
	//MPChar strUrl = "bipbopall.m3u8";

	//解析m3u8
	if (!ParseM3u8(strUrl))
	{
		return MFalse;
	}

	//判断上面解析的m3u8是否为嵌套,是嵌套则去解析新的
	Playlist* playlist = MNull;
	if (m_playlistList.GetSize() > 1 || m_playlistList.GetLastNode()->segmentList.GetSize() == 0)
	{
		
		playlist = m_playlistList.GetLastNode();
		if (!ParseM3u8(playlist->strUrl, playlist))
		{
			return MFalse;
		}
	}
	m_bTsReady = MTrue;
	if (playlist->segmentList.GetSize() > 0)
	{
		if (m_dataRead->Open(playlist->segmentList.GetLastNode()->url))
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

			m_pTs = TsStream::read_probe(tmpBuffer, PROBE_BUFFER_SIZE);
			if (m_pTs == MNull)
			{
				return MFalse;
			}
			m_dataRead->Close();
			m_pTs->SetDataRead(m_dataRead);
			m_pTs->ReadHeader(playlist->segmentList.GetLastNode()->url);

			return MTrue;
		}
		
	}



	return MTrue;
}

MBool ParseHls::ReadPacket()
{
	if (m_pTs)
	{
		m_pTs->ReadPacket();
	}
	return MFalse;
}

void ParseHls::ff_parse_key_val_cb(void* srcData, MPChar key, MInt32 keyLen, MPChar value)
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

IParse* ParseHls::hls_probe(MPChar p_buffer, MUInt32 p_size)
{
	if (MStrNCmp(p_buffer, "#EXTM3U", 7) == 0) {
		return new ParseHls();
	}
		

	return MNull;
}





MBool ParseHls::ParseM3u8(MPChar strUrl, Playlist* playlist)
{


	if (m_dataRead == MNull)
	{
		return MFalse;
	}

	MInt32 max_buffer_size = 1024 * 8;
	MChar buffer[1024*8] = { 0 };
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
	MPChar  bufferEnd = MNull;
	MInt32	bufferSize = 0;

	MInt64 duration = 0;

	

	MInt32 bufferReadSize = 0;
	MPChar curBufferPos = MNull;


	if (!m_dataRead->Open(strUrl))
	{
		return MFalse;
	}

	MBool ret = MFalse;

	MInt32 iBufferSize = MAX_URL_SIZE;
	while (true)
	{

		curBufferPos = buffer;
		iBufferSize = max_buffer_size;
		MBool retRead = MFalse;
		do
		{

			retRead = m_dataRead->Read(&curBufferPos, iBufferSize, bufferReadSize);
			curBufferPos += bufferReadSize;
			iBufferSize -= bufferReadSize;
				

		} while (retRead && iBufferSize > 0);

		if (!retRead && curBufferPos == buffer)
		{
			break;
		}

		bufferEnd = curBufferPos;
		curBufferPos = buffer;
		bufferReadSize = bufferEnd - buffer;
		while (curBufferPos<bufferEnd)
		{
			lineSize = ToolString::Read_line(curBufferPos, bufferReadSize, line, MAX_URL_SIZE);
			if (!lineSize)
			{
				goto Exit;
				//return MTrue;
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
				ToolString::ff_parse_key_value(ptr, ParseHls::ff_parse_key_val_cb, playlist);

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
				playlist = createPlaylist(playlist);
				if (playlist == MNull)
				{
					// return MFalse;
					goto Exit;
				}

			}
			else if (ToolString::av_strstart(line, "#EXT-X-MEDIA-SEQUENCE:", &ptr)) {
				//暂时忽略
				playlist = createPlaylist(playlist);
				if (playlist == MNull)
				{
					//return MFalse;
					goto Exit;
				}
			}
			else if (ToolString::av_strstart(line, "#EXT-X-PLAYLIST-TYPE:", &ptr)) {
				//暂时忽略
				playlist = createPlaylist(playlist);
				if (playlist == MNull)
				{
					//return MFalse;
					goto Exit;
				}
			}
			else if (ToolString::av_strstart(line, "#EXT-X-MAP:", &ptr)) {
				//暂时忽略
				playlist = createPlaylist(playlist);
				if (playlist == MNull)
				{
					//return MFalse;
					goto Exit;
				}
			}
			else if (ToolString::av_strstart(line, "#EXT-X-ENDLIST", &ptr)) {
				break;
			}
			else if (ToolString::av_strstart(line, "#EXTINF:", &ptr)) {
				is_segment = 1;
				duration = MStoi64(ptr) * AV_TIME_BASE;
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
					MInt32 strNameSize = sizeof(line) + sizeof(strUrl) + 1;
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
					ToolString::ff_make_absolute_url(tmp_strUrl, sizeof(tmp_strUrl), strUrl, line);

					seg->url = ToolString::av_strdup(tmp_strUrl);
					if (!seg->url) {
						MMemFree(MNull, seg);
						goto Exit;
						//return MFalse;
					}


					is_segment = 0;
					playlist->segmentList.AddNode(seg);
				}



			}

			isFirst = MFalse;
		}
	}

	ret = MTrue;
Exit:	
	m_dataRead->Close();

	return ret;
}