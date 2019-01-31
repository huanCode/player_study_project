#pragma once
#ifndef _AVCONFIG_H
#define _AVCONFIG_H

enum AV_MediaType {
	AV_MEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
	AV_MEDIA_TYPE_VIDEO,
	AV_MEDIA_TYPE_AUDIO,
	AV_MEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
	AV_MEDIA_TYPE_SUBTITLE,
	AV_MEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
	AV_MEDIA_TYPE_NB
};

enum AV_CodecID {
	_AV_CODEC_ID_NONE,
	_AV_CODEC_ID_AAC,
	_AV_CODEC_ID_H264
};


#endif //_AVCONFIG_H