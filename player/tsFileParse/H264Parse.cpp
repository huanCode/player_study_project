#include "stdafx.h"
#include "H264Parse.h"
#include "ammem.h"
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


static const MUInt8 default_scaling4[2][16] =
{
	{ 6,13,20,28,
	13,20,28,32,
	20,28,32,37,
	28,32,37,42
	},{
		10,14,20,24,
		14,20,24,27,
		20,24,27,30,
		24,27,30,34
	}
};

static const MUInt8 default_scaling8[2][64] =
{
	{ 6,10,13,16,18,23,25,27,
	10,11,16,18,23,25,27,29,
	13,16,18,23,25,27,29,31,
	16,18,23,25,27,29,31,33,
	18,23,25,27,29,31,33,36,
	23,25,27,29,31,33,36,38,
	25,27,29,31,33,36,38,40,
	27,29,31,33,36,38,40,42
	},{
		9,13,15,17,19,21,22,24,
		13,13,17,19,21,22,24,25,
		15,17,19,21,22,24,25,27,
		17,19,21,22,24,25,27,28,
		19,21,22,24,25,27,28,30,
		21,22,24,25,27,28,30,32,
		22,24,25,27,28,30,32,33,
		24,25,27,28,30,32,33,35
	}
};


static const MUInt8 zigzag_scan[16] = {
	0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
	1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
	1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
	3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

static const MUInt8 zigzag_scan8x8[64] = {
	0 + 0 * 8, 1 + 0 * 8, 0 + 1 * 8, 0 + 2 * 8,
	1 + 1 * 8, 2 + 0 * 8, 3 + 0 * 8, 2 + 1 * 8,
	1 + 2 * 8, 0 + 3 * 8, 0 + 4 * 8, 1 + 3 * 8,
	2 + 2 * 8, 3 + 1 * 8, 4 + 0 * 8, 5 + 0 * 8,
	4 + 1 * 8, 3 + 2 * 8, 2 + 3 * 8, 1 + 4 * 8,
	0 + 5 * 8, 0 + 6 * 8, 1 + 5 * 8, 2 + 4 * 8,
	3 + 3 * 8, 4 + 2 * 8, 5 + 1 * 8, 6 + 0 * 8,
	7 + 0 * 8, 6 + 1 * 8, 5 + 2 * 8, 4 + 3 * 8,
	3 + 4 * 8, 2 + 5 * 8, 1 + 6 * 8, 0 + 7 * 8,
	1 + 7 * 8, 2 + 6 * 8, 3 + 5 * 8, 4 + 4 * 8,
	5 + 3 * 8, 6 + 2 * 8, 7 + 1 * 8, 7 + 2 * 8,
	6 + 3 * 8, 5 + 4 * 8, 4 + 5 * 8, 3 + 6 * 8,
	2 + 7 * 8, 3 + 7 * 8, 4 + 6 * 8, 5 + 5 * 8,
	6 + 4 * 8, 7 + 3 * 8, 7 + 4 * 8, 6 + 5 * 8,
	5 + 6 * 8, 4 + 7 * 8, 5 + 7 * 8, 6 + 6 * 8,
	7 + 5 * 8, 7 + 6 * 8, 6 + 7 * 8, 7 + 7 * 8,
};

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
	if (leading_zero_bits > 24) leading_zero_bits = 24;
	code = (1 << leading_zero_bits) - 1;
	return code + (nalu_get_bits(pNalu, leading_zero_bits + 1) & (code));
}


MInt32 se_utility(NAL_UNIT *pNALU)
{
	MUInt32 code_num = ue_utility(pNALU);
	MInt32 result;
	result = (code_num + 1) >> 1;
	if (!(code_num & 1))
		result = -result;
	return result;
}




static void decode_scaling_list(NAL_UNIT *pNalu, MUInt8 *factors, int size,
	const MUInt8 *jvt_list, const MUInt8 *fallback_list)
{
	int i, last = 8, next = 8;
	const MUInt8 *scan = size == 16 ? zigzag_scan : zigzag_scan8x8;
	if (!nalu_get_bit(pNalu)) /* matrix not written, we use the predicted one */
		MMemCpy(factors, fallback_list, size * sizeof(MUInt8));
	else
		for (i = 0; i < size; i++) {
			if (next)
				next = (last + se_utility(pNalu)) & 0xff;
			if (!i && !next) { /* matrix not written, we use the preset one */
				MMemCpy(factors, jvt_list, size * sizeof(MUInt8));
				break;
			}
			last = factors[scan[i]] = next ? next : last;
		}
}











static void decode_scaling_matrices(NAL_UNIT *pNalu, int is_sps,
	MUInt8(*scaling_matrix4)[16], MUInt8(*scaling_matrix8)[64])
{
	//H264Context * s = h;
	int fallback_sps = !is_sps;
	const MUInt8 *fallback[4];
	fallback[0] = fallback_sps ? scaling_matrix4[0] : default_scaling4[0];
	fallback[1] = fallback_sps ? scaling_matrix4[3] : default_scaling4[1];
	fallback[2] = fallback_sps ? scaling_matrix8[0] : default_scaling8[0];
	fallback[3] = fallback_sps ? scaling_matrix8[1] : default_scaling8[1];

	if (nalu_get_bit(pNalu))
	{
		decode_scaling_list(pNalu, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]); // Intra, Y
		decode_scaling_list(pNalu, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
		decode_scaling_list(pNalu, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
		decode_scaling_list(pNalu, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]); // Inter, Y
		decode_scaling_list(pNalu, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
		decode_scaling_list(pNalu, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
		if (is_sps) {
			decode_scaling_list(pNalu, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]);  // Intra, Y
			decode_scaling_list(pNalu, scaling_matrix8[1], 64, default_scaling8[1], fallback[3]);  // Inter, Y
		}
	}
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


MBool H264Parse::GetWidthAndHeight(MByte* pBitstream, MDWord dwBitstreamLen, MInt32& w, MInt32& h)
{
	NAL_UNIT NALU;
	NAL_UNIT *pNalu = &NALU;
	MUInt16 val;
	MByte* ptr = pBitstream;
	MByte* end = pBitstream + dwBitstreamLen;
	MDWord leng = dwBitstreamLen;
	MUInt32 i, flag0, flag1, flag2, flag3, frame_mbs_only_flag;
	MInt32 iWidddth, iHeiiith, profile_idc;
	MDWord uCode = 0;
	MDWord iFindSlice = 0;
	MUInt8 scaling_matrix4[6][16];
	MUInt8 scaling_matrix8[2][64];

	MBool mWidthCorpIncorrect = MFalse;
	MBool mHeightCorpIncorrect = MFalse;
	MDWord dwWidth = 0;
	MDWord dwHeight = 0;

	if (end - ptr < 3)
	{
		return MFalse;
	}
	ptr = pBitstream + 2;
	uCode = (ptr[-2] << 8) | ptr[-1];


	while (ptr < end)
	{
		uCode = ((uCode << 8) | ptr[0]) & 0xFFFFFF;
		if (uCode == 1 && ((ptr[1] & 0x1f) == 0x7))
		{
			//find sps
			iFindSlice = 1;
			break;
		}
		ptr++;
	}

	if (iFindSlice == 0)
	{
		return MFalse;
	}

	ptr++;

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

	if (pNalu->nal_unit_type != NAL_UNIT_TYPE_SPS)
	{
		return MFalse;
	}

	//parsing sps
	//profile_idc
	profile_idc = nalu_get_bits(pNalu, 8);
	if (profile_idc != 66 && profile_idc != 77 && profile_idc != 100
		&& profile_idc != 110 && profile_idc != 122 && profile_idc != 144)
		return MFalse;

	flag0 = nalu_get_bit(pNalu);
	flag1 = nalu_get_bit(pNalu);
	flag2 = nalu_get_bits(pNalu, 2);
	i = nalu_get_bits(pNalu, 4);		/* reserved_zero_5bits */
	if (i != 0)
		return MFalse;

	//level_idc
	i = nalu_get_bits(pNalu, 8);		// level
										//sps_id
	i = ue_utility(pNalu);
	if (i > 31)
		return MFalse;

	if (profile_idc >= 100)
	{
		i = ue_utility(pNalu);
		if (i == 3) //chroma_format_idc
		{
			i = nalu_get_bit(pNalu);  //residual_color_transform_flag
		}
		i = ue_utility(pNalu);  //bit_depth_luma_minus8
		i = ue_utility(pNalu);  //bit_depth_chroma_minus8
		i = nalu_get_bit(pNalu); // 
		decode_scaling_matrices(pNalu, 1, scaling_matrix4, scaling_matrix8);

	}


	//log2_max_frame_num_minus4
	i = ue_utility(pNalu);
	if (i > 12)
		return MFalse;

	//pic_order_cnt_type
	i = ue_utility(pNalu);
	if (i == 0)
	{
		//log2_max_pic_order_cnt_lsb_minus4
		i = ue_utility(pNalu);
		if (i > 12)
			return MFalse;
	}
	else if (i == 1)
	{
		//delta_pic_order_always_zero_flag
		i = nalu_get_bit(pNalu);
		//offset_for_non_ref_pic
		i = se_utility(pNalu);
		//offset_for_top_to_bottom_field
		i = se_utility(pNalu);
		//num_ref_frames_in_pic_order_cnt_cycle
		flag0 = ue_utility(pNalu);
		if (flag0 > 255)
			return MFalse;

		for (i = 0; i < flag0; i++) {
			//offset_for_ref_frame
			flag1 = se_utility(pNalu);
		}
	}

	//num_ref_frames
	i = ue_utility(pNalu);
	if (i > 16)
		return MFalse;
	//gaps_in_frame_num_value_allowed_flag
	i = nalu_get_bit(pNalu);
	//pic_width_in_mbs_minus1
	iWidddth = ue_utility(pNalu);
	//pic_height_in_mbs_minus1
	iHeiiith = ue_utility(pNalu);
	//frame_mbs_only_flag
	frame_mbs_only_flag = nalu_get_bit(pNalu);
	if (frame_mbs_only_flag == 0) {
		//mb_adaptive_frame_field_flag
		i = nalu_get_bit(pNalu);
	}
	//direct_8x8_inference_flag
	i = nalu_get_bit(pNalu);
	//frame_cropping_flag
	i = nalu_get_bit(pNalu);
	if (i)
	{
		// NOTE: only 4:2:0 format supported now.
		//frame_crop_left_offset
		flag0 = ue_utility(pNalu) << 1;
		//frame_crop_right_offset
		flag1 = ue_utility(pNalu) << 1;
		//frame_crop_top_offset
		flag2 = (ue_utility(pNalu) << 1)* (2 - frame_mbs_only_flag);
		//frame_crop_bottom_offset
		flag3 = (ue_utility(pNalu) << 1)* (2 - frame_mbs_only_flag);
	}
	else
	{
		//frame_crop_left_offset
		flag0 = 0;
		//frame_crop_right_offset
		flag1 = 0;
		//frame_crop_top_offset
		flag2 = 0;
		//frame_crop_bottom_offset
		flag3 = 0;
	}

	{
		MDWord dwWidthmbs = (1 + iWidddth) << 4;
		MDWord dwHeightmbs = ((1 + iHeiiith) << 4) * (2 - frame_mbs_only_flag);
		dwWidth = ((1 + iWidddth) << 4) - (flag0 + flag1);
		dwHeight = ((1 + iHeiiith) << 4) * (2 - frame_mbs_only_flag) - (flag2 + flag3);

		if (dwWidth <= 0 || dwHeight <= 0)
			return MFalse;

		w = dwWidth;
		h = dwHeight;


	}

	return MTrue;

}


