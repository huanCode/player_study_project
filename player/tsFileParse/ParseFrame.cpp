#include "stdafx.h"
#include "ParseFrame.h"
#include "ParseHls.h"
#include "IParse.h"
ParseFrame::ParseFrame()
{
	//m_probeList.AddNode(ParseHls::hls_probe);
	m_parse = MNull;
}

MBool	ParseFrame::HasVideo()
{
	if (m_parse)
	{
		return m_parse->HasVideo();
	}

	return MFalse;
}

MBool	ParseFrame::HasAudio()
{
	if (m_parse)
	{
		return m_parse->HasAudio();
	}

	return MFalse;
}


MBool ParseFrame::FindParse(MPChar pbuf, MInt32 iBufSize)
{
	read_probe probeFuntion = MNull;
	m_parse = ParseHls::hls_probe(pbuf, iBufSize);
	return m_parse ? MTrue : MFalse;
}

MVoid	ParseFrame::SetDataRead(IDataRead* obj)
{
	if (m_parse)
	{
		m_parse->SetDataRead(obj);
	}
}


MBool	ParseFrame::ReadHeader(MPChar strUrl)
{
	return m_parse->ReadHeader(strUrl);
}

MBool ParseFrame::ReadFrame(AVPkt** pkt)
{
	if (m_parse)
	{
		return m_parse->ReadPacket(pkt);
	}
	return MFalse;
}

MBool ParseFrame::Seek(MInt64 seekTimeStamp)
{
	return m_parse->Seek(seekTimeStamp);
}