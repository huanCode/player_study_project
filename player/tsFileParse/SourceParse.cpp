#include "stdafx.h"
#include "SourceParse.h"
#include "ammem.h"
#define BUFFER_SIZE		(1024*32)

SourceParse::SourceParse()
{
	m_pBuffer = MNull;
	m_iBufferCanReadSize = 0;

	m_hasAudio = MTrue;
	m_hasVideo = MFalse;

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

			MDWord timeDuration = 0;
			MDWord timeBegin = MGetCurTimeStamp();
			if (m_parseFrame.FindParse(m_pBuffer, m_iBufferCanReadSize))
			{
				m_sourceFrame.Close();
				m_parseFrame.SetDataRead(&m_sourceFrame);
				//if (m_sourceFrame.Open(strUrl))
				//{
				timeDuration = MGetCurTimeStamp() - timeBegin;
				printf("SourceParse::Open 0 time = %d ms\r\n", timeDuration);
				MBool ret  = m_parseFrame.ReadHeader(strUrl);
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

MBool SourceParse::Seek(MInt64 seekTimeStamp)
{
	return m_parseFrame.Seek(seekTimeStamp);
}
