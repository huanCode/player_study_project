#pragma once
#include "amcomdef.h"
#include "mv3File.h"
#include "TsStreamDef.h"
//#include "tsFilter.h"
#include "IParse.h"
#include "common.h"
#include "H264Parse.h"
#define PACKET_SIZE	188
#define FILTER_NUM	6
#define PROBE_BUFFER_SIZE	204 * 10
using namespace std;
class tsFilter;
class TsStream:public IParse
{
public:
	TsStream();
private:
	struct Packet
	{
		MInt64	pts;
		MInt64  dts;
		MByte*	buffer;
		MUInt32	size;
		MInt32	type;
	};


	struct ts_packet_header
	{
		ts_packet_header()
		{
			sync_byte = 0;
			transport_error = MFalse;
			bStart_payload = MFalse;
			pid = 0;
			has_adaptation = MFalse;
			has_payload = MFalse;
			continuity_counter = 0;
		}
		MUInt8	sync_byte;
		bool transport_error;
		bool	bStart_payload;
		MUInt16	pid;
		bool has_adaptation;
		bool has_payload;
		MUInt8 continuity_counter;
	};

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




	enum MpegTSState {
		MPEGTS_HEADER = 0,
		MPEGTS_PESHEADER,
		MPEGTS_PESHEADER_FILL,
		MPEGTS_PAYLOAD,
		MPEGTS_SKIP,
	};

public:
	MUInt32 mpegts_read_header();
	//解析ts
	static IParse*	read_probe(MPChar p_buffer, MUInt32 p_size);
	//MInt32	read_header(MPChar p_buffer, MUInt32 p_size);


	MBool	ReadHeader(MPChar strUrl);
	MBool	ReadPacket(AVPkt** pkt);
	MVoid	Close();

private:
	MBool	Init();


	MBool handle_packets(MInt32 nb_packets = 1000);
	MBool handle_packet(MPChar pBuffer);


	tsFilter*	add_filter(MInt32 type,MInt32 pid);
	tsFilter*	get_filter(MInt32 pid);
	MVoid	Release();

	MInt32 get_packet_size(MPChar buf, MInt32 size);
	MInt32 analyze(MPChar buf, MInt32 size, MInt32 packet_size, MInt32 probe);
private:




	MVoid parse_ts_packet_header(MPChar buffer, ts_packet_header &tsHeader);

	




private:
	MUInt32		m_pcr_pid;


	MpegTSState	m_pes_state;
	Packet		m_packet;
	bool		m_bReadOneFrame;	//表示是否成功读完一帧
	MUInt32		m_frame_total_size;
	MUInt32		m_pes_header_size;
	MByte		m_buffer[512000];
	mv3File		m_fileWrite;

	MBool		m_isStart;
	//friend class tsFilter;
	friend class tsSectionPat;
	friend class tsSectionPmt;
	friend class tsSectionPes;
	tsFilter*	m_filter[FILTER_NUM];


	MPChar		m_packetBuffer;
	MInt32		m_iReadSize = 0;


	static MInt32	Packet_Size;

	MInt32		m_trackNum;		//有几条流

	MInt32		m_stopParse;

	AVPkt			m_avpkt;

	mv3File file;
	mv3File audioFile;

	MInt32	m_width;
	MInt32	m_height;
public:




};
