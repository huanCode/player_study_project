#include "stdafx.h"
#include "DemuxerMgr.h"
#include "DemuxerM3u8.h"
#include "IDemuxer.h"
DemuxerMgr::DemuxerMgr()
{
	//m_probeList.AddNode(ParseHls::hls_probe);
	m_parse = MNull;
}

MBool	DemuxerMgr::HasVideo()
{
	if (m_parse)
	{
		return m_parse->HasVideo();
	}

	return MFalse;
}

MBool	DemuxerMgr::HasAudio()
{
	if (m_parse)
	{
		return m_parse->HasAudio();
	}

	return MFalse;
}


MBool DemuxerMgr::FindParse(MPChar pbuf, MInt32 iBufSize)
{
	read_probe probeFuntion = MNull;
	m_parse = DemuxerM3u8::hls_probe(pbuf, iBufSize);
	return m_parse ? MTrue : MFalse;
}

MVoid	DemuxerMgr::SetDataRead(IStreamRead* obj)
{
	if (m_parse)
	{
		m_parse->SetDataRead(obj);
	}
}


MBool	DemuxerMgr::ReadHeader(MPChar strUrl)
{
	return m_parse->ReadHeader(strUrl);
}

MBool DemuxerMgr::ReadFrame(AVPkt** pkt)
{
	if (m_parse)
	{
		return m_parse->ReadPacket(pkt);
	}
	return MFalse;
}

MBool DemuxerMgr::Seek(MInt64 seekTimeStamp)
{
	return m_parse->Seek(seekTimeStamp);
}