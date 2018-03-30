#pragma once
#ifndef _PARSEFRAME_H
#define _PARSEFRAME_H
#include "amcomdef.h"
#include "IBaseIo.h"
#include "ToolList.h"

class IParse;
class ParseFrame
{
public:
	typedef MBool(*read_probe)(MPChar p_buffer, MUInt32 p_size);
	ParseFrame();

	MBool FindParse(MPChar pbuf,MInt32 iBufSize);
	MBool	ReadHeader();
	MVoid	SetDataRead(IDataRead* obj);
private:
	IParse*	m_parse;
	//ToolList<read_probe>	m_probeList;

};




#endif // !_PARSEFRAME_H