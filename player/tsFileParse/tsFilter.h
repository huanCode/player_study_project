#include "amcomdef.h"
#include "TsStreamDef.h"
#define FILTER_NUM	6
class DemuxerTs;
class tsFilter
{
public:
	enum MpegTSFilterType {
		MPEGTS_PES,
		MPEGTS_SECTION,
		MPEGTS_PCR,
	};
	tsFilter()
	{
		m_pid = -1;
		m_es_id = -1;
		m_last_cc = -1;
		m_last_pcr = -1;
		m_description = MNull;
		m_last_version = -1;
	}
	virtual MUInt32 parse(DemuxerTs* p_tsStream, MPChar p_buffer,MUInt32 p_buffer_size) = 0;
	virtual MVoid	SetPid(MInt32 p_pid) = 0;


	MInt32 GetPid() { return m_pid; };
	MpegTSFilterType GetType() {
		return m_type;
	}

private:


protected:
	typedef struct SectionHeader {
		SectionHeader()
		{
			tid = 0;
			section_length = 0;
			id = 0;
			version = 0;
			sec_num = 0;
			last_sec_num = 0;

		}
		MUInt8 tid;
		MUInt8 section_length;
		MUInt16 id;
		MUInt8 version;
		MUInt8 sec_num;
		MUInt8 last_sec_num;
	} SectionHeader;
	MInt32 parse_section_header(MPChar buffer_section_header, SectionHeader &section_header);
	MBool skip_identical(MInt32 iVersion, MInt32 iCrc = 0 /*暂时未用*/);
private:
	 
	MInt32 m_es_id;
	MInt32 m_last_cc; /* last cc code (-1 if first packet) */
	MInt64 m_last_pcr;
protected:
	MInt32 m_pid;
	MPChar	m_description;
	MInt32	m_last_version;
	enum MpegTSFilterType m_type;

};

class tsSection :public tsFilter
{
public:
	tsSection();
	MVoid write_section_data(DemuxerTs* p_tsStream, const MPChar p_buf, MUInt32 p_buf_size, MBool p_is_start);
protected:
	MInt32 m_section_index;
	MInt32 m_section_length;
	MInt32 m_last_ver;
	MUInt32 m_crc;
	MUInt32 m_last_crc;
	MPChar  m_section_buf;
	MBool	m_end_of_section_reached;
};


class tsSectionSdt :public tsSection
{
public:
	tsSectionSdt()
	{
		m_type = MPEGTS_SECTION;
		m_pid = SDT_PID;
		m_description = "sdt";
	}
	MUInt32 parse(DemuxerTs* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size) { return 0; };
	MVoid	SetPid(MInt32 p_pid) {};
};


class tsSectionPat:public tsSection
{
public:
	tsSectionPat()
	{
		m_type = MPEGTS_SECTION;
		m_pid = PAT_PID;
		m_description = "pat";
	}
	MUInt32 parse(DemuxerTs* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {};
};




class tsSectionPmt :public tsSection
{
public:
	tsSectionPmt()
	{
		m_type = MPEGTS_SECTION;
		m_description = "pmt";
	}
	MUInt32 parse(DemuxerTs* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {
		m_pid = p_pid;
	};
private:
	MBool is_pes_stream(int stream_type, MUInt32 prog_reg_desc = 0);

private:
	MInt32	m_pcr_pid;
};
//能知道流类型:video、audio
class tsSectionPes :public tsFilter
{
public:
	tsSectionPes()
	{
		m_type = MPEGTS_PES;
		m_description = "pes";
		m_mediaType = AV_MediaType::AV_MEDIA_TYPE_UNKNOWN;
		m_mediaCodecID = AV_CodecID::_AV_CODEC_ID_NONE;
		m_state = MpegTSState::MPEGTS_SKIP;
		m_header_size = 0;
		m_stream_id = 0;
		m_total_size = 0;
		m_buffer = MNull;
		m_pes_header_size = 0;

		m_pts = 0;
		m_dts = 0;
		m_stream_type = 0;
		m_buffer_size = 0;
		m_curIndex = -1;
		m_count = 0;
		m_flags = false;

	}
	MUInt32 parse(DemuxerTs* p_tsStream, MPChar p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {
		m_pid = p_pid;
	};

	MVoid SetIndex(MInt32 iIndex)
	{
		m_curIndex = iIndex;
	};

	//获取流类型video、audio
	MBool mpegts_find_stream_type(MInt32 stream_type, const StreamType *types);
private:
	inline MInt64 ff_parse_pes_pts(const MUInt8 *buffer);
	MVoid resetPesInfo()
	{
		m_pts = -1;
		m_dts = -1;
		m_buffer_size = 0;
	}
private:
	AV_MediaType	m_mediaType;
	AV_CodecID		m_mediaCodecID;
	MpegTSState		m_state;

	MInt32			m_header_size;
	MUInt8			m_header[MAX_PES_HEADER_SIZE];

	MUInt32			m_stream_id;
	MUInt32			m_total_size;
	MPChar			m_buffer;
	MInt32			m_buffer_size;
	MInt32			m_pes_header_size;

	MInt64			m_pts;
	MInt64			m_dts;
	MInt32			m_stream_type;

	MInt32			m_curIndex;
	MInt32			m_count;

	MBool			m_flags;	//true : key frame





};


class FilterFactory
{
public:
	static tsFilter* CreateFilter(MInt32 type, MInt32 pid);

};

