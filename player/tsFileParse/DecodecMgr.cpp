#include "DecodecMgr.h"
#include "TsStreamDef.h"
#include "DecodeH264.h"
#include "DecodeAAC.h"
DecodecMgr::DecodecMgr()
	:m_decode(MNull)
{

}


MBool DecodecMgr::Open(MInt32 iDecodeType)
{
	if (m_decode)
	{
		return MTrue;
	}

	switch (iDecodeType)
	{
	//------------------Video-------------------------
	case _AV_CODEC_ID_H264:
		m_decode = new DecodeH264();
		break;
	//------------------Audio-------------------------
	case _AV_CODEC_ID_AAC:
		m_decode = new DecodeAAC();
		break;
	default:
		break;
	}

	if (m_decode)
	{
		return m_decode->Open();
	}



	return MFalse;

}


MVoid DecodecMgr::Close()
{
	if (m_decode)
	{
		m_decode->Close();
	}
}



Frame*	DecodecMgr::DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize, MInt64 pts, MInt64 dts)
{
	if (m_decode)
	{
		return m_decode->DecodeFrame(srcBuffer, srcBufferSize, pts, dts);
	}

	return MNull;
}