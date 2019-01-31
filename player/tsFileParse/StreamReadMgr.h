#pragma once
#ifndef _STREAMREADMGR_H
#define _STREAMREADMGR_H
#include "amcomdef.h"
#include "ToolList.h"
#include "ToolBuffer.h"
#include "IStreamRead.h"

enum IBaseIoType
{
	none,
	Http,
	File,
	Rtmp
};


class IBaseIo;
class StreamReadMgr:public IStreamRead
{
public:


	StreamReadMgr();
	
	MBool Open(MPChar strUrl);
	MBool Read(MChar** pBuf, MDWord dwSize,MInt32& out_readSize);
	MVoid GetConfig(MInt32 dwCfgType, MVoid** pdwValue);
	//MInt32 IoReadLine(MChar** ppBuffer);
	MBool isFinish(){return m_isFinish;};
	MVoid Close();
private:
	MBool createBaseIo(MPChar strUrl);
	IBaseIoType parseUrl(MPChar strUrl);
	MBool findParse(MPChar buffer,MInt32 bufSize);

	//IoBase读取的数据填满buffer
	MBool fillBuffer();
private:
	IBaseIoType		m_baseIoType;
	IBaseIo*		m_baseIo;
	ToolList<IBaseIo*>	m_baseIoList;
	ToolBuffer		m_buffer;
	MBool			m_isFinish;


};




#endif // !_STREAMREADMGR_H
