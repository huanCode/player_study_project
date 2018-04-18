#include "stdafx.h"
#include "SourceParse.h"
#include "ammem.h"
#define BUFFER_SIZE		(1024*32)

SourceParse::SourceParse()
{
	m_pBuffer = MNull;
	m_iBufferCanReadSize = 0;
}

MBool SourceParse::Open(MPChar strUrl)
{
	if (strUrl == MNull)
	{
		return MFalse;
	}

	m_pBuffer = (MPChar)MMemAlloc(MNull, BUFFER_SIZE);
	if (m_pBuffer == MNull)
	{
		return MFalse;
	}
	MMemSet(m_pBuffer, 0, BUFFER_SIZE);

	if (m_sourceFrame.Open(strUrl))
	{
		
		if (m_sourceFrame.Read(&m_pBuffer, BUFFER_SIZE, m_iBufferCanReadSize))
		{
			
			if (m_parseFrame.FindParse(m_pBuffer, m_iBufferCanReadSize))
			{
				m_sourceFrame.Close();
				m_parseFrame.SetDataRead(&m_sourceFrame);
				//if (m_sourceFrame.Open(strUrl))
				//{

				return m_parseFrame.ReadHeader(strUrl);
				//}

				
			}
		}
	}
	return MFalse;
}




MBool SourceParse::ReadFrame(AVPkt** pkt)
{
	return m_parseFrame.ReadFrame(pkt);

}
