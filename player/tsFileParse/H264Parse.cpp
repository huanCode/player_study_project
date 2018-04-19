#include "stdafx.h"
#include "H264Parse.h"

#define SWAP16( n )				((MUInt16)((n<<8)|(n>>8)))

#define NAL_UNIT_TYPE_SPS                  7
#define NAL_UNIT_TYPE_SLICE                1
#define NAL_UNIT_TYPE_IDR                  5

#define LOAD_WORDS_X(reg, left, ptr)			\
	if( left <= 0 )	{							\
	MUInt16 words =*(ptr)++;	MInt32 shiftBitnow = -left;	\
	reg |=((MUInt32)SWAP16(words))<<shiftBitnow;	\
	left += 16;								\
	}

static MInt32 nalu_show_bits(NAL_UNIT *pNALU, MInt32 n)
{
	MInt32 shiftbitnow = 32 - n; return (MUInt16)(pNALU->reg >> shiftbitnow);
}

static void nalu_flush_bits(NAL_UNIT* pNALU, MInt32 n)
{
	pNALU->reg <<= n;
	pNALU->rem -= n;
	LOAD_WORDS_X(pNALU->reg, pNALU->rem, pNALU->ptr);
}

static MUInt32 nalu_get_bits(NAL_UNIT* pNALU, MInt32 n)
{
	MInt32 shiftBitNow = 32 - n;
	MUInt16 val = (MUInt16)(pNALU->reg >> shiftBitNow);
	pNALU->reg <<= n;
	pNALU->rem -= n;
	LOAD_WORDS_X(pNALU->reg, pNALU->rem, pNALU->ptr);

	return val;
}

static MUInt32 nalu_get_bit(NAL_UNIT* pNALU)
{
	MUInt16 val = (MUInt16)(pNALU->reg >> 31);
	pNALU->reg <<= 1;
	pNALU->rem--;
	LOAD_WORDS_X(pNALU->reg, pNALU->rem, pNALU->ptr);

	return val;
}

static const MUInt8 exp_golomb_tbl[] = {
	8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static MUInt32 ue_utility(NAL_UNIT *pNalu)
{
	MInt32 leading_zero_bits = 0;
	MUInt32 code = 0;
	while (!(code = nalu_show_bits(pNalu, 8))) {
		leading_zero_bits += 8;
		nalu_flush_bits(pNalu, 8);
		if (leading_zero_bits == 24) break;
	}
	leading_zero_bits += exp_golomb_tbl[code];
	nalu_flush_bits(pNalu, exp_golomb_tbl[code]);
	if (leading_zero_bits>24) leading_zero_bits = 24;
	code = (1 << leading_zero_bits) - 1;
	return code + (nalu_get_bits(pNalu, leading_zero_bits + 1) & (code));
}


MDWord H264Parse::GetSliceType(MByte* pBitstream, MDWord dwBitstreamLen)
{
	NAL_UNIT NALU;
	NAL_UNIT *pNalu = &NALU;

	MUInt16 val;
	MByte* ptr = pBitstream;
	MByte* end = pBitstream + dwBitstreamLen;
	MDWord leng = dwBitstreamLen;
	MDWord i;
	MDWord uCode = 0;
	MDWord iFindSlice = 0;

	if (end - ptr < 3)
	{
		return AMC_H264_UTL_ERR_PARAM;
	}
	ptr = pBitstream + 2;
	uCode = (ptr[-2] << 8) | ptr[-1];

	while (ptr < end)
	{
		uCode = ((uCode << 8) | ptr[0]) & 0xFFFFFF;
		if (uCode == 1 && ((ptr[1] & 0x1f) == 0x1 || (ptr[1] & 0x1f) == 0x5))
		{
			//find slice
			iFindSlice = 1;
			break;
		}
		ptr++;
	}

	/*do {
	uCode = ((uCode << 8) | ptr[0]) & 0xFFFFFF;
	} while (uCode != 1 && ptr++ < end);*/

	ptr++;

	//if (ptr > end)
	if (iFindSlice == 0)
	{
		return AMC_H264_UTL_ERR_PARAM;
	}


	if ((MPtrVar)ptr & 1)
	{
		val = (*ptr) << 8;
		pNalu->reg = (MUInt32)SWAP16(val) << 16; //!< first 8 bits.
		pNalu->rem = 8;
		ptr++;
		val = *(MUInt16*)ptr;
		pNalu->reg |= SWAP16(val);		//!< second 16 bits.
		pNalu->reg <<= 8;
	}
	else
	{
		val = *(MUInt16*)ptr;
		pNalu->reg = (MUInt32)SWAP16(val) << 16;	//!< first 16 bits
		pNalu->rem = 16;
		ptr += 2;
		val = *(MUInt16*)ptr;
		pNalu->reg |= SWAP16(val);		//!< second 16 bits.
	}

	pNalu->ptr = (MUInt16*)(ptr + 2);
	pNalu->len = leng;

	//parsing nal head	
	if (pNalu->len > 3)
	{
		pNalu->forbidden_zero_bit = nalu_get_bit(pNalu);
		pNalu->nal_ref_idc = nalu_get_bits(pNalu, 2);
		pNalu->nal_unit_type = nalu_get_bits(pNalu, 5);
	}
	else
	{
		pNalu->nal_unit_type = 0;
	}

	if (pNalu->nal_unit_type != NAL_UNIT_TYPE_SLICE&&pNalu->nal_unit_type != NAL_UNIT_TYPE_IDR)
	{
		return AMC_H264_UTL_ERR_PARAM;
	}

	if (pNalu->nal_unit_type == NAL_UNIT_TYPE_IDR)
	{
		return IDR_SLICE;
	}

	i = ue_utility(pNalu);
	i = ue_utility(pNalu);
	if (i > 4)
		i -= 5;

	if (i != P_SLICE&&i != B_SLICE&&i != I_SLICE)
	{
		return AMC_H264_UTL_ERR_PARAM;
	}

	return i;

}


