#pragma once
#ifndef _H264PARSE_H
#define _H264PARSE_H
#include "amcomdef.h"

#define P_SLICE			0
#define B_SLICE			1
#define I_SLICE			2
#define IDR_SLICE		5

#define AMC_H264_UTL_OK							0			//success
#define AMC_H264_UTL_ERR_BASE					0x8000     

#define AMC_H264_UTL_ERR_PARAM					(AMC_H264_UTL_ERR_BASE+0)

typedef struct _NAL_UNIT
{
	// ISO/IEC 14496-10 part 7.3.1
	MUInt32			forbidden_zero_bit;			// should be always FALSE
	MUInt32			nal_ref_idc;				// NAL_UNIT_PRIORITY_xxxx
	MUInt32			nal_unit_type;				// NAL_UNIT_TYPE_xxxx
												// pCtx

												// global bitstream buffer.
	MUInt8			*buffer;					// buffer pointer
	MUInt8			*end;						// end bitstream ptr

												// rbsp layer
	MUInt8			*rbsp_start;				// rbsp start pointer (with 00 00 01 header)
	MInt32			len;						// length of rbsp data
	MInt32			used;						// count of emulation_prevention_three_byte

												// ptr decoding pCtx.
												// 	u8			*ptr;						// current bitstream ptr.
												// 	int			offset;						// bit offset.

	MUInt32			reg;			//!< bits_reg( remain_bits( curr_word ) + next_word )
	MInt32			rem;			//!< bits_len( the remain of the curr word ); (16~1)
	MUInt16			*ptr;			//!< word_pos( the word bits that will load );

} NAL_UNIT;

class H264Parse
{
public:


public:
	static MDWord GetSliceType(MByte* pBitstream, MDWord dwBitstreamLen);
	static MBool GetWidthAndHeight(MByte* pBitstream, MDWord dwBitstreamLen, MInt32& w, MInt32& h);

};



#endif // !_H264PARSE_H