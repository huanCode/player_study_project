#pragma once
#ifndef _DEMUXER_HLS_H
#define _DEMUXER_HLS_H
#include "amcomdef.h"
#include "ToolList.h"
#include "IDemuxer.h"
struct segment {
	MInt64 duration;
	MInt64 url_offset;
	MInt64 size;
	MPChar url;

	/* associated Media Initialization Section, treated as a segment */
	struct segment *init_section;
};







class DemuxerM3u8:public IDemuxer
{
private:
	class Playlist
	{
	public:
		Playlist()
		{
			iBandwidth = 0;
			iDuration = 0;
			iCurrentIndex = 0;
			strUrl = MNull;
			
			//iSegments = 0;
			//ts = MNull;
		}
		void AddSegment(segment* ts);

		MPChar	strUrl;
		MInt32	iBandwidth;
		MInt32	iDuration;
		MInt32	iCurrentIndex;
		ToolList<segment*> segmentList;
		//MInt32	iSegments;
		//struct segment *ts;
	};

public:
	DemuxerM3u8();


	MBool	ReadHeader(MPChar strUrl);
	MBool	ReadPacket(AVPkt** pkt);
	MVoid	Close();
	MBool   Seek(MInt64 seekTimeStamp);
	MBool	HasVideo()
	{
		if (m_pTs)
		{
			return m_pTs->HasVideo();
		}

		return MFalse;
	}

	MBool	HasAudio()
	{
		if (m_pTs)
		{
			return m_pTs->HasAudio();
		}

		return MFalse;
	}

	static IDemuxer* hls_probe(MPChar p_buffer, MUInt32 p_size);

	MBool ParseM3u8(MPChar strUrl,Playlist* playlist = MNull);
private:
	MBool read_line();

	MBool switchSegment();
private:
	enum m3u8Level
	{
		TopLevel,
		SecondLevel
	};





	Playlist* createPlaylist(Playlist* playlist)
	{
		if (playlist == MNull)
		{
			playlist = new Playlist();
		}

		return playlist;
	}


private:
	static void ff_parse_key_val_cb(void* srcData, MPChar key, MInt32 keyLen, MPChar value);

private:

	ToolList<Playlist*>		m_playlistList;
	MBool					m_playListType;
	MInt32					m_curIndex;
	MInt64					m_duration;


	IDemuxer*		m_pTs;
	MBool		m_bTsReady;

	MInt32		m_curSementIndex;
	Playlist*	m_curPlaylist;


	MInt64			m_beginDts;
	MInt64			m_beginPts;
	MBool			m_bFirst;
};




#endif // !_DEMUXER_HLS_H