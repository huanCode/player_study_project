#include "stdafx.h"
#include "StreamReadMgr.h"
#include "HttpIo.h"
#include "amstring.h"
#include "ToolString.h"

#include "common.h"
#include <string.h>
#include "PlatformConfig.h"
#define URL_SCHEME_CHARS                        \
    "abcdefghijklmnopqrstuvwxyz"                \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                \
    "0123456789+-."

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
static inline int is_dos_path(const char *path)
{
#if HAVE_DOS_PATHS
	if (path[0] && path[1] == ':')
		return 1;
#endif
	return 0;
}


StreamReadMgr::StreamReadMgr()
{
	m_baseIoType = none;
	m_isFinish = MFalse;




}


MBool StreamReadMgr::Open(MPChar strUrl)
{
	if (strUrl == MNull)
	{
		return MFalse;
	}

	if (!createBaseIo(strUrl))
	{
		return MFalse;
	}

	if (!m_buffer.Create())
	{
		return MFalse;
	}

	return m_baseIo->IoOpen(strUrl);
	 
}


MVoid StreamReadMgr::Close()
{
	m_baseIo->IoClose();
}


MVoid StreamReadMgr::GetConfig(MInt32 dwCfgType, MVoid** pdwValue)
{
	if (m_baseIo)
	{
		m_baseIo->IoGetConfig(dwCfgType, pdwValue);
	}
}

MBool StreamReadMgr::Read(MChar** pBuf, MDWord dwSize, MInt32& out_readSize)
{
	if (pBuf == MNull || dwSize <=0)
	{
		return 0;
	}
	out_readSize = 0;
	MPChar buffer = *pBuf;
	MInt32 readSize = 0;
	while (dwSize)
	{
		readSize = m_baseIo->IoRead(buffer + out_readSize,dwSize);
		if (readSize == dwSize)
		{
			out_readSize += readSize;
			return MTrue;
		}
		else if (readSize < dwSize && readSize > 0)
		{
			out_readSize += readSize;
			dwSize -= readSize;
		}
		else if (readSize == -1)
		{
			if (!m_baseIo->isComplete())
			{
				int a = 1;
			}

			//表明没有数据了,
			return MTrue;
		}
		else if (readSize == 0)
		{
			MSleep(5);
		}
		
	}
	if (dwSize < 0)
	{
		return MFalse;
	}

	return MTrue;
}


//MInt32 SourceFrame::IoReadLine(MChar** ppBuffer)
//{
//	if (*ppBuffer == MNull)
//	{
//		return 0;
//	}
//
//
//}

IBaseIoType StreamReadMgr::parseUrl(MPChar strUrl)
{

	char proto_str[128], proto_nested[128], *ptr;
	size_t proto_len = strspn(strUrl, URL_SCHEME_CHARS);
	int i;

	if (strUrl[proto_len] != ':' &&
		(strncmp(strUrl, "subfile,", 8) || !strchr(strUrl + proto_len + 1, ':')) ||
		is_dos_path(strUrl))
		strcpy(proto_str, "file");
	else
		ToolString::av_strlcpy(proto_str, strUrl,
			FFMIN(proto_len + 1, sizeof(proto_str)));

	//if ((ptr = strchr(proto_str, ',')))
	//	*ptr = '\0';
	//ToolString::av_strlcpy(proto_nested, proto_str, sizeof(proto_nested));
	//if ((ptr = strchr(proto_nested, '+')))
	//	*ptr = '\0';

	IBaseIoType type = IBaseIoType::none;
	if (proto_str)
	{
		if (!MStrCmp(proto_str, "http"))
		{
			type = IBaseIoType::Http;
		}
		else if (!MStrCmp(proto_str, "file"))
		{
			type = IBaseIoType::File;
		}
		else if (!MStrCmp(proto_str, "rtmp"))
		{
			type = IBaseIoType::Rtmp;
		}
	}

	return type;

}


MBool StreamReadMgr::findParse(MPChar buffer, MInt32 bufSize)
{
	return MFalse;
}

MBool StreamReadMgr::fillBuffer()
{
	while (m_buffer.GetWriteSize())
	{

	}

	return MFalse;
}

MBool StreamReadMgr::createBaseIo(MPChar strUrl)
{
	IBaseIoType type = parseUrl(strUrl);

	if (type == IBaseIoType::Http)
	{
		m_baseIo = new HttpIo();
	}

	return m_baseIo ? MTrue : MFalse;
}