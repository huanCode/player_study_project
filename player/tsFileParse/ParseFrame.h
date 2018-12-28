#pragma once
#ifndef _PARSEFRAME_H
#define _PARSEFRAME_H
#include "amcomdef.h"
#include "IBaseIo.h"
#include "ToolList.h"
#include "common.h"
class IDataRead;
class IParse;
class ParseFrame
{
public:
	typedef MBool(*read_probe)(MPChar p_buffer, MUInt32 p_size);
	ParseFrame();

	MBool FindParse(MPChar pbuf,MInt32 iBufSize);
	MBool	ReadHeader(MPChar strUrl);

	MBool ReadFrame(AVPkt** pkt);
	MVoid	SetDataRead(IDataRead* obj);

	MBool	HasVideo()
	{
		if (m_parse)
		{
			m_parse->HasVideo();
		}

		return MFalse;
	}

	MBool	HasAudio()
	{
		if (m_parse)
		{
			m_parse->HasAudio();
		}

		return MFalse;
	}

private:
	IParse*	m_parse;
	//ToolList<read_probe>	m_probeList;

};




#endif // !_PARSEFRAME_H