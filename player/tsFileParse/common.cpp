#include "stdafx.h"
#include "common.h"

inline MUInt16 to_UInt16(MPChar p)
{
	MUInt16 n = ((MPChar)p)[0];
	n <<= 8;
	n += ((MPChar)p)[1]; return n;
}
inline MUInt8 to_UInt8(MPChar p)
{
	return *((MPChar)p);
}

MUInt8 get8(MChar*& p)
{
	MPChar tmp = p;
	p++;
	return to_UInt8(tmp);

}

MUInt16 get16(MChar*& p)
{
	MPChar tmp = p;
	p += 2;
	return to_UInt16(tmp);
}
