/*
*
*	Sony PlayStation (1994)
*
*	Geometry Transformation Engine
*
*
*	CREDIT:
* 
*		https://github.com/SonofUgly/PCSX-Reloaded
*
*		https://github.com/Gh0stBlade/libValkyrie
* 
*		https://github.com/OpenDriver2/PsyCross
*
*/


#include "lib_gte.h"

#include "PSYQ/INLINE_C.H"

#include "PSYQ/GTEMAC.H"

#include "PSYQ/rcossin_tbl.h"

#include "PSYQ/rsin_tbl.h"

#include "PSYQ/ratan_tbl.h"

#include "PSYQ/sqrt_tbl.h"

#include "PSYQ/sqrt_inv_tbl.h"

#include <iostream>

#include <cassert>


/*
*	Data Registers mtc2 mfc2
*/
#define C2_VX0		(CP2D.p[ 0 ].sw.l)
#define C2_VY0		(CP2D.p[ 0 ].sw.h)
#define C2_VZ0		(CP2D.p[ 1 ].sw.l)
#define C2_VX1		(CP2D.p[ 2 ].w.l)
#define C2_VY1		(CP2D.p[ 2 ].w.h)
#define C2_VZ1		(CP2D.p[ 3 ].w.l)
#define C2_VX2		(CP2D.p[ 4 ].w.l)
#define C2_VY2		(CP2D.p[ 4 ].w.h)
#define C2_VZ2		(CP2D.p[ 5 ].w.l)
#define C2_R		(CP2D.p[ 6 ].b.l)
#define C2_G		(CP2D.p[ 6 ].b.h)
#define C2_B		(CP2D.p[ 6 ].b.h2)
#define C2_CODE		(CP2D.p[ 6 ].b.h3)
#define C2_OTZ		(CP2D.p[ 7 ].w.l)
#define C2_IR0		(CP2D.p[ 8 ].sw.l)
#define C2_IR1		(CP2D.p[ 9 ].sw.l)
#define C2_IR2		(CP2D.p[ 10 ].sw.l)
#define C2_IR3		(CP2D.p[ 11 ].sw.l)
#define C2_SXY0		(CP2D.p[ 12 ].d)
#define C2_SX0		(CP2D.p[ 12 ].sw.l)
#define C2_SY0		(CP2D.p[ 12 ].sw.h)
#define C2_SXY1		(CP2D.p[ 13 ].d)
#define C2_SX1		(CP2D.p[ 13 ].sw.l)
#define C2_SY1		(CP2D.p[ 13 ].sw.h)
#define C2_SXY2		(CP2D.p[ 14 ].d)
#define C2_SX2		(CP2D.p[ 14 ].sw.l)
#define C2_SY2		(CP2D.p[ 14 ].sw.h)
#define C2_SXYP		(CP2D.p[ 15 ].d)
#define C2_SXP		(CP2D.p[ 15 ].sw.l)
#define C2_SYP		(CP2D.p[ 15 ].sw.h)
#define C2_SZ0		(CP2D.p[ 16 ].w.l)
#define C2_SZ1		(CP2D.p[ 17 ].w.l)
#define C2_SZ2		(CP2D.p[ 18 ].w.l)
#define C2_SZ3		(CP2D.p[ 19 ].w.l)
#define C2_RGB0		(CP2D.p[ 20 ].d)
#define C2_R0		(CP2D.p[ 20 ].b.l)
#define C2_G0		(CP2D.p[ 20 ].b.h)
#define C2_B0		(CP2D.p[ 20 ].b.h2)
#define C2_CD0		(CP2D.p[ 20 ].b.h3)
#define C2_RGB1		(CP2D.p[ 21 ].d)
#define C2_R1		(CP2D.p[ 21 ].b.l)
#define C2_G1		(CP2D.p[ 21 ].b.h)
#define C2_B1		(CP2D.p[ 21 ].b.h2)
#define C2_CD1		(CP2D.p[ 21 ].b.h3)
#define C2_RGB2		(CP2D.p[ 22 ].d)
#define C2_R2		(CP2D.p[ 22 ].b.l)
#define C2_G2		(CP2D.p[ 22 ].b.h)
#define C2_B2		(CP2D.p[ 22 ].b.h2)
#define C2_CD2		(CP2D.p[ 22 ].b.h3)
#define C2_RES1		(CP2D.p[ 23 ].d)
#define C2_MAC0		(CP2D.p[ 24 ].sd)
#define C2_MAC1		(CP2D.p[ 25 ].sd)
#define C2_MAC2		(CP2D.p[ 26 ].sd)
#define C2_MAC3		(CP2D.p[ 27 ].sd)
#define C2_IRGB		(CP2D.p[ 28 ].d)
#define C2_ORGB		(CP2D.p[ 29 ].d)
#define C2_LZCS		(CP2D.p[ 30 ].d)
#define C2_LZCR		(CP2D.p[ 31 ].d)

/*
*	Control Registers ctc2 cfc2
*/
#define C2_R11		(CP2C.p[ 0 ].sw.l)
#define C2_R12		(CP2C.p[ 0 ].sw.h)
#define C2_R13		(CP2C.p[ 1 ].sw.l)
#define C2_R21		(CP2C.p[ 1 ].sw.h)
#define C2_R22		(CP2C.p[ 2 ].sw.l)
#define C2_R23		(CP2C.p[ 2 ].sw.h)
#define C2_R31		(CP2C.p[ 3 ].sw.l)
#define C2_R32		(CP2C.p[ 3 ].sw.h)
#define C2_R33		(CP2C.p[ 4 ].sw.l)
#define C2_TRX		(CP2C.p[ 5 ].sd)
#define C2_TRY		(CP2C.p[ 6 ].sd)
#define C2_TRZ		(CP2C.p[ 7 ].sd)
#define C2_L11		(CP2C.p[ 8 ].sw.l)
#define C2_L12		(CP2C.p[ 8 ].sw.h)
#define C2_L13		(CP2C.p[ 9 ].sw.l)
#define C2_L21		(CP2C.p[ 9 ].sw.h)
#define C2_L22		(CP2C.p[ 10 ].sw.l)
#define C2_L23		(CP2C.p[ 10 ].sw.h)
#define C2_L31		(CP2C.p[ 11 ].sw.l)
#define C2_L32		(CP2C.p[ 11 ].sw.h)
#define C2_L33		(CP2C.p[ 12 ].sw.l)
#define C2_RBK		(CP2C.p[ 13 ].sd)
#define C2_GBK		(CP2C.p[ 14 ].sd)
#define C2_BBK		(CP2C.p[ 15 ].sd)
#define C2_LR1		(CP2C.p[ 16 ].sw.l)
#define C2_LR2		(CP2C.p[ 16 ].sw.h)
#define C2_LR3		(CP2C.p[ 17 ].sw.l)
#define C2_LG1		(CP2C.p[ 17 ].sw.h)
#define C2_LG2		(CP2C.p[ 18 ].sw.l)
#define C2_LG3		(CP2C.p[ 18 ].sw.h)
#define C2_LB1		(CP2C.p[ 19 ].sw.l)
#define C2_LB2		(CP2C.p[ 19 ].sw.h)
#define C2_LB3		(CP2C.p[ 20 ].sw.l)
#define C2_RFC		(CP2C.p[ 21 ].sd)
#define C2_GFC		(CP2C.p[ 22 ].sd)
#define C2_BFC		(CP2C.p[ 23 ].sd)
#define C2_OFX		(CP2C.p[ 24 ].sd)
#define C2_OFY		(CP2C.p[ 25 ].sd)
#define C2_H		(CP2C.p[ 26 ].sw.l)
#define C2_DQA		(CP2C.p[ 27 ].sw.l)
#define C2_DQB		(CP2C.p[ 28 ].sd)
#define C2_ZSF3		(CP2C.p[ 29 ].sw.l)
#define C2_ZSF4		(CP2C.p[ 30 ].sw.l)
#define C2_FLAG		(CP2C.p[ 31 ].d)

/*
*	Data Macros
*/
#define VX(n)				(n < 3 ? CP2D.p[ n << 1 ].sw.l : C2_IR1)
#define VY(n)				(n < 3 ? CP2D.p[ n << 1 ].sw.h : C2_IR2)
#define VZ(n)				(n < 3 ? CP2D.p[ (n << 1) + 1 ].sw.l : C2_IR3)
#define MX11(n)				(n < 3 ? CP2C.p[ (n << 3) ].sw.l : -C2_R << 4)
#define MX12(n)				(n < 3 ? CP2C.p[ (n << 3) ].sw.h : C2_R << 4)
#define MX13(n)				(n < 3 ? CP2C.p[ (n << 3) + 1 ].sw.l : C2_IR0)
#define MX21(n)				(n < 3 ? CP2C.p[ (n << 3) + 1 ].sw.h : C2_R13)
#define MX22(n)				(n < 3 ? CP2C.p[ (n << 3) + 2 ].sw.l : C2_R13)
#define MX23(n)				(n < 3 ? CP2C.p[ (n << 3) + 2 ].sw.h : C2_R13)
#define MX31(n)				(n < 3 ? CP2C.p[ (n << 3) + 3 ].sw.l : C2_R22)
#define MX32(n)				(n < 3 ? CP2C.p[ (n << 3) + 3 ].sw.h : C2_R22)
#define MX33(n)				(n < 3 ? CP2C.p[ (n << 3) + 4 ].sw.l : C2_R22)
#define CV1(n)				(n < 3 ? CP2C.p[ (n << 3) + 5 ].sd : 0)
#define CV2(n)				(n < 3 ? CP2C.p[ (n << 3) + 6 ].sd : 0)
#define CV3(n)				(n < 3 ? CP2C.p[ (n << 3) + 7 ].sd : 0)

/*
*	Opcode
*/
#define gteop(code)			(code & 0x1ffffff)
#define GTE_SF(op)			((op >> 19) & 1)
#define GTE_MX(op)			((op >> 17) & 3)
#define GTE_V(op)			((op >> 15) & 3)
#define GTE_CV(op)			((op >> 13) & 3)
#define GTE_LM(op)			((op >> 10) & 1)
#define GTE_FUNCT(op)		(op & 63)

/*
*	Min/Max
*/
#ifndef max
#   define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#   define min(a, b) ((a) < (b) ? (a) : (b))
#endif
inline static int fst_min(int a, int b)
{
	int diff = a - b;
	int dsgn = diff >> 31;
	return b + (diff & dsgn);
}

inline static int fst_max(int a, int b)
{
	int diff = a - b;
	int dsgn = diff >> 31;
	return a - (diff & dsgn);
}
#ifndef MIN
#define MIN(a,b)	fst_min(a,b)
#endif

#ifndef MAX
#define MAX(a,b)	fst_max(a,b)
#endif

/*
*	
*/
#define	FIXED(a)	((a) >> 12)


/*
*   Static Variables
*/
int Sony_PlayStation_GTE::m_sf = 0;
long long Sony_PlayStation_GTE::m_mac0 = 0;
long long Sony_PlayStation_GTE::m_mac3 = 0;


/*
	Precision Geometry Transform Pipeline
*/
void Sony_PlayStation_GTE::PGXP_ClearCache(void)
{
	g_pgxpVertexIndex = 0;
}

uint16_t Sony_PlayStation_GTE::PGXP_GetIndex(int checkTransform)
{
	if (!checkTransform || g_pgxpTransformed)
	{
		if (checkTransform)
			g_pgxpTransformed = 0;
		return g_pgxpVertexIndex;
	}

	return 0xFFFF;
}

uint16_t Sony_PlayStation_GTE::PGXP_EmitCacheData(PGXPVData* newData)
{
	uint16_t nextIndex = g_pgxpVertexIndex++;

	if (nextIndex == 0xffff)
		return 0xffff;

	g_pgxpCache[nextIndex] = *newData;
	g_pgxpTransformed = 1;
	return nextIndex;
}

void Sony_PlayStation_GTE::PGXP_SetZOffsetScale(float offset, float scale)
{
	g_pgxpZOffset = offset;
	g_pgxpZScale = scale;
}

int Sony_PlayStation_GTE::PGXP_GetCacheData(PGXPVData* out, uint32_t lookup, uint16_t indexhint) const
{
	if (indexhint == 0xFFFF)
	{
		out->px = 0.0f;
		out->py = 0.0f;
		out->pz = 1.0f;
		out->scr_h = 0.0f;
		out->ofx = 0.0f;
		out->ofx = 0.0f;
		return 0;
	}

	// index hint allows us to start from specific index
	uint16_t index = max(0, int(indexhint) - 8);

	for (int i = 0; i < 512; i++)
	{
		if (index == 0xffff)
			index++;

		if (g_pgxpCache[index].lookup == lookup)
		{
			if (i > 256)
			std::cout << "PGXP_GetCacheData lookup IS inefficient: hint: %d, start: %d, found: %d, cycles: %d\n" << indexhint << uint16_t(indexhint - 8u) << index << i << std::endl;

			*out = g_pgxpCache[index];
			return 1;
		}
		index++;
	}

	std::cout << "PGXP_GetCacheData lookup IS NOT FOUND: hint: %d\n" << indexhint << std::endl;

	out->px = 0.0f;
	out->py = 0.0f;
	out->pz = 1.0f;
	out->scr_h = 0.0f;
	out->ofx = 0.0f;
	out->ofx = 0.0f;

	return 0;
}


/*
	GTE Emulator
*/
unsigned int Sony_PlayStation_GTE::gte_leadingzerocount(unsigned int lzcs)
{
	if (!lzcs)
		return 32;

	// perform fast bit scan

	unsigned int lzcr = lzcs;
	static char debruijn32[32] = {
		0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
		1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
	};

	lzcr |= lzcr >> 1;
	lzcr |= lzcr >> 2;
	lzcr |= lzcr >> 4;
	lzcr |= lzcr >> 8;
	lzcr |= lzcr >> 16;
	lzcr++;

	return debruijn32[lzcr * 0x076be629 >> 27];
}

inline long long Sony_PlayStation_GTE::gte_shift(long long a, int sf)
{
	if (sf > 0)
		return a >> 12;
	else if (sf < 0)
		return a << 12;

	return a;
}

unsigned int Sony_PlayStation_GTE::gte_divide(unsigned short numerator, unsigned short denominator)
{
	if (numerator < (denominator * 2))
	{
		static unsigned char table[] =
		{
			0xff, 0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf3, 0xf1, 0xef, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe3,
			0xe1, 0xdf, 0xdd, 0xdc, 0xda, 0xd8, 0xd6, 0xd5, 0xd3, 0xd1, 0xd0, 0xce, 0xcd, 0xcb, 0xc9, 0xc8,
			0xc6, 0xc5, 0xc3, 0xc1, 0xc0, 0xbe, 0xbd, 0xbb, 0xba, 0xb8, 0xb7, 0xb5, 0xb4, 0xb2, 0xb1, 0xb0,
			0xae, 0xad, 0xab, 0xaa, 0xa9, 0xa7, 0xa6, 0xa4, 0xa3, 0xa2, 0xa0, 0x9f, 0x9e, 0x9c, 0x9b, 0x9a,
			0x99, 0x97, 0x96, 0x95, 0x94, 0x92, 0x91, 0x90, 0x8f, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x87, 0x86,
			0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x75, 0x74,
			0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64,
			0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5d, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55,
			0x54, 0x53, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e, 0x4d, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x48,
			0x47, 0x46, 0x45, 0x44, 0x43, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3f, 0x3e, 0x3d, 0x3c, 0x3c, 0x3b,
			0x3a, 0x39, 0x39, 0x38, 0x37, 0x36, 0x36, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2f,
			0x2e, 0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2a, 0x2a, 0x29, 0x28, 0x28, 0x27, 0x26, 0x26, 0x25, 0x24,
			0x24, 0x23, 0x22, 0x22, 0x21, 0x20, 0x20, 0x1f, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c, 0x1b, 0x1b, 0x1a,
			0x19, 0x19, 0x18, 0x18, 0x17, 0x16, 0x16, 0x15, 0x15, 0x14, 0x14, 0x13, 0x12, 0x12, 0x11, 0x11,
			0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x08, 0x08,
			0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00,
			0x00
		};

		int shift = gte_leadingzerocount(denominator) - 16;

		int r1 = (denominator << shift) & 0x7fff;
		int r2 = table[((r1 + 0x40) >> 7)] + 0x101;
		int r3 = ((0x80 - (r2 * (r1 + 0x8000))) >> 8) & 0x1ffff;
		unsigned int reciprocal = ((r2 * r3) + 0x80) >> 8;

		return (unsigned int)((((unsigned long long)reciprocal * (numerator << shift)) + 0x8000) >> 16);
	}

	return 0xffffffff;
}

int Sony_PlayStation_GTE::GTE_RotTransPers(int idx, int lm)
{
	int h_over_sz3;

	C2_MAC1 = A1((long long)((long long)C2_TRX << 12) + (C2_R11 * VX(idx)) + (C2_R12 * VY(idx)) + (C2_R13 * VZ(idx)));
	C2_MAC2 = A2((long long)((long long)C2_TRY << 12) + (C2_R21 * VX(idx)) + (C2_R22 * VY(idx)) + (C2_R23 * VZ(idx)));
	C2_MAC3 = A3((long long)((long long)C2_TRZ << 12) + (C2_R31 * VX(idx)) + (C2_R32 * VY(idx)) + (C2_R33 * VZ(idx)));
	C2_IR1 = Lm_B1(C2_MAC1, lm);
	C2_IR2 = Lm_B2(C2_MAC2, lm);
	C2_IR3 = Lm_B3_sf(m_mac3, m_sf, lm);
	C2_SZ0 = C2_SZ1;
	C2_SZ1 = C2_SZ2;
	C2_SZ2 = C2_SZ3;
	C2_SZ3 = Lm_D(m_mac3, 1);
	h_over_sz3 = Lm_E(gte_divide(C2_H, C2_SZ3));
	C2_SXY0 = C2_SXY1;
	C2_SXY1 = C2_SXY2;
	C2_SX2 = Lm_G1(F((long long)C2_OFX + ((long long)C2_IR1 * h_over_sz3)) >> 16);
	C2_SY2 = Lm_G2(F((long long)C2_OFY + ((long long)C2_IR2 * h_over_sz3)) >> 16);

	if (b_PGXP)
	{
		// perform the same but in floating point
		double fMAC1 = ((double)((float)C2_TRX * ONE) + ((float)C2_R11 * (float)VX(idx)) + ((float)C2_R12 * (float)VY(idx)) + ((float)C2_R13 * (float)VZ(idx)));
		double fMAC2 = ((double)((float)C2_TRY * ONE) + ((float)C2_R21 * (float)VX(idx)) + ((float)C2_R22 * (float)VY(idx)) + ((float)C2_R23 * (float)VZ(idx)));
		double fMAC3 = ((double)((float)C2_TRZ * ONE) + ((float)C2_R31 * (float)VX(idx)) + ((float)C2_R32 * (float)VY(idx)) + ((float)C2_R33 * (float)VZ(idx)));

		const double one_by_v = 1.0 / (512.0 * 1024.0);

		g_FP_SXYZ0 = g_FP_SXYZ1;
		g_FP_SXYZ1 = g_FP_SXYZ2;

		// do not perform perspective multiplication so it stays in object space
		// perspective is performed exclusively in shader
		PGXPVector3D temp;
		temp.px = fMAC1 * one_by_v * g_pgxpZScale + g_pgxpZOffset;
		temp.py = fMAC2 * one_by_v * g_pgxpZScale + g_pgxpZOffset;
		temp.pz = fMAC3 * one_by_v * g_pgxpZScale + g_pgxpZOffset;

		// calculate projected values for cache
		temp.x = (double(C2_OFX) + double(float(C2_IR1) * float(h_over_sz3))) / float(1 << 16);
		temp.y = (double(C2_OFY) + double(float(C2_IR2) * float(h_over_sz3))) / float(1 << 16);
		temp.z = float(max(C2_SZ3, C2_H / 2)) / float(1 << 16);

		g_FP_SXYZ2 = temp;

		PGXPVData vdata;
		vdata.lookup = PGXP_LOOKUP_VALUE(temp.x, temp.y);		// hash short values

		// FIXME: actually we scaling here entire geometry, is that correct?
		vdata.px = temp.px;
		vdata.py = temp.py;
		vdata.pz = temp.pz;

		vdata.ofx = float(C2_OFX) / float(1 << 16);
		vdata.ofy = float(C2_OFY) / float(1 << 16);
		vdata.scr_h = float(C2_H);

		PGXP_EmitCacheData(&vdata);
	}

	return h_over_sz3;
}

int Sony_PlayStation_GTE::LIM(int value, int max, int min, unsigned int flag)
{
	if (value > max) {
		C2_FLAG |= flag;
		return max;
	}
	else if (value < min) {
		C2_FLAG |= flag;
		return min;
	}

	return value;
}

int Sony_PlayStation_GTE::BOUNDS(long long value, int max_flag, int min_flag)
{
	if (value > (long long)0x7ffffffffff)
		C2_FLAG |= max_flag;

	if (value < (long long)-0x8000000000)
		C2_FLAG |= min_flag;

	return int(gte_shift(value, m_sf));
}

unsigned int Sony_PlayStation_GTE::MFC2(int reg)
{
	switch (reg) {
	case 1:
	case 3:
	case 5:
	case 8:
	case 9:
	case 10:
	case 11:
		CP2D.p[reg].d = (int)CP2D.p[reg].sw.l;
		break;

	case 7:
	case 16:
	case 17:
	case 18:
	case 19:
		CP2D.p[reg].d = (unsigned int)CP2D.p[reg].w.l;
		break;

	case 15:
		CP2D.p[reg].d = C2_SXY2;
		break;

	case 28:
	case 29:
		CP2D.p[reg].d = LIM(C2_IR1 >> 7, 0x1f, 0, 0) | (LIM(C2_IR2 >> 7, 0x1f, 0, 0) << 5) | (LIM(C2_IR3 >> 7, 0x1f, 0, 0) << 10);
		break;
	}

	return CP2D.p[reg].d;
}

unsigned int Sony_PlayStation_GTE::CFC2(int reg)
{
	return CP2C.p[reg].d;
}

int Sony_PlayStation_GTE::MFC2_S(int reg)
{
	// FIXME: Is that modifiers should be signed too?
	switch (reg) {
	case 1:
	case 3:
	case 5:
	case 8:
	case 9:
	case 10:
	case 11:
		CP2D.p[reg].d = (int)CP2D.p[reg].sw.l;
		break;

	case 7:
	case 16:
	case 17:
	case 18:
	case 19:
		CP2D.p[reg].d = (unsigned int)CP2D.p[reg].w.l;
		break;

	case 15:
		CP2D.p[reg].d = C2_SXY2;
		break;

	case 28:
	case 29:
		CP2D.p[reg].d = LIM(C2_IR1 >> 7, 0x1f, 0, 0) | (LIM(C2_IR2 >> 7, 0x1f, 0, 0) << 5) | (LIM(C2_IR3 >> 7, 0x1f, 0, 0) << 10);
		break;
	}

	return CP2D.p[reg].sd;
}

int Sony_PlayStation_GTE::CFC2_S(int reg)
{
	return CP2C.p[reg].sd;
}

void Sony_PlayStation_GTE::MTC2(unsigned int value, int reg)
{
	switch (reg) {
	case 15:
		C2_SXY0 = C2_SXY1;
		C2_SXY1 = C2_SXY2;
		C2_SXY2 = value;
		break;

	case 28:
		C2_IR1 = (value & 0x1f) << 7;
		C2_IR2 = (value & 0x3e0) << 2;
		C2_IR3 = (value & 0x7c00) >> 3;
		break;

	case 30:
		C2_LZCR = gte_leadingzerocount(value);
		break;

	case 31:
		return;
	}

	CP2D.p[reg].d = value;
}

void Sony_PlayStation_GTE::CTC2(unsigned int value, int reg)
{
	switch (reg) {
	case 4:
	case 12:
	case 20:
	case 26:
	case 27:
	case 29:
	case 30:
		value = (int)(short)value;
		break;

	case 31:
		value = value & 0x7ffff000;
		if ((value & 0x7f87e000) != 0)
			value |= 0x80000000;
		break;
	}

	CP2C.p[reg].d = value;
}

void Sony_PlayStation_GTE::MTC2_S(int value, int reg)
{
	switch (reg) {
	case 15:
		C2_SXY0 = C2_SXY1;
		C2_SXY1 = C2_SXY2;
		C2_SXY2 = value;
		break;

	case 28:
		C2_IR1 = (value & 0x1f) << 7;
		C2_IR2 = (value & 0x3e0) << 2;
		C2_IR3 = (value & 0x7c00) >> 3;
		break;

	case 30:
		C2_LZCR = gte_leadingzerocount(value);
		break;

	case 31:
		return;
	}

	CP2D.p[reg].sd = value;
}

void Sony_PlayStation_GTE::CTC2_S(int value, int reg)
{
	switch (reg) {
	case 4:
	case 12:
	case 20:
	case 26:
	case 27:
	case 29:
	case 30:
		value = (int)(short)value;
		break;

	case 31:
		value = value & 0x7ffff000;
		if ((value & 0x7f87e000) != 0)
			value |= 0x80000000;
		break;
	}

	CP2C.p[reg].sd = value;
}

int Sony_PlayStation_GTE::A1(long long a)
{
	return BOUNDS(a, (1 << 31) | (1 << 30), (1 << 31) | (1 << 27));
}

int Sony_PlayStation_GTE::A2(long long a)
{
	return BOUNDS(a, (1 << 31) | (1 << 29), (1 << 31) | (1 << 26));
}

int Sony_PlayStation_GTE::A3(long long a)
{
	m_mac3 = a; return BOUNDS(a, (1 << 31) | (1 << 28), (1 << 31) | (1 << 25));
}

int Sony_PlayStation_GTE::Lm_B1(int a, int lm)
{
	return LIM(a, 0x7fff, -0x8000 * !lm, (1 << 31) | (1 << 24));
}

int Sony_PlayStation_GTE::Lm_B2(int a, int lm)
{
	return LIM(a, 0x7fff, -0x8000 * !lm, (1 << 31) | (1 << 23));
}

int Sony_PlayStation_GTE::Lm_B3(int a, int lm)
{
	return LIM(a, 0x7fff, -0x8000 * !lm, (1 << 22));
}

int Sony_PlayStation_GTE::Lm_B3_sf(long long value, int sf, int lm)
{
	int value_sf = int(gte_shift(value, sf));
	int value_12 = int(gte_shift(value, 1));
	int max = 0x7fff;
	int min = 0;
	if (lm == 0)
		min = -0x8000;

	if (value_12 < -0x8000 || value_12 > 0x7fff)
		C2_FLAG |= (1 << 22);

	if (value_sf > max)
		return max;
	else if (value_sf < min)
		return min;

	return value_sf;
}

int Sony_PlayStation_GTE::Lm_C1(int a)
{
	return LIM(a, 0x00ff, 0x0000, (1 << 21));
}

int Sony_PlayStation_GTE::Lm_C2(int a)
{
	return LIM(a, 0x00ff, 0x0000, (1 << 20));
}

int Sony_PlayStation_GTE::Lm_C3(int a)
{
	return LIM(a, 0x00ff, 0x0000, (1 << 19));
}

int Sony_PlayStation_GTE::Lm_D(long long a, int sf)
{
	return LIM(int(gte_shift(a, sf)), 0xffff, 0x0000, (1 << 31) | (1 << 18));
}

unsigned int Sony_PlayStation_GTE::Lm_E(unsigned int result)
{
	if (result == 0xffffffff) {
		C2_FLAG |= (1 << 31) | (1 << 17);
		return 0x1ffff;
	}

	if (result > 0x1ffff)
		return 0x1ffff;

	return result;
}

long long Sony_PlayStation_GTE::F(long long a)
{
	m_mac0 = a;

	if (a > 0x7fffffffLL)
		C2_FLAG |= (1 << 31) | (1 << 16);

	if (a < -0x80000000LL)
		C2_FLAG |= (1 << 31) | (1 << 15);

	return a;
}

int Sony_PlayStation_GTE::Lm_G1(long long a)
{
	if (a > 0x3ff) {
		C2_FLAG |= (1 << 31) | (1 << 14);
		return 0x3ff;
	}
	if (a < -0x400) {
		C2_FLAG |= (1 << 31) | (1 << 14);
		return -0x400;
	}

	return int(a);
}

int Sony_PlayStation_GTE::Lm_G2(long long a)
{
	if (a > 0x3ff) {
		C2_FLAG |= (1 << 31) | (1 << 13);
		return 0x3ff;
	}

	if (a < -0x400) {
		C2_FLAG |= (1 << 31) | (1 << 13);
		return -0x400;
	}

	return int(a);
}

int Sony_PlayStation_GTE::Lm_G1_ia(long long a)
{
	if (a > 0x3ffffff)
		return 0x3ffffff;

	if (a < -0x4000000)
		return -0x4000000;

	return int(a);
}

int Sony_PlayStation_GTE::Lm_G2_ia(long long a)
{
	if (a > 0x3ffffff)
		return 0x3ffffff;

	if (a < -0x4000000)
		return -0x4000000;

	return int(a);
}

int Sony_PlayStation_GTE::Lm_H(long long value, int sf)
{
	long long value_sf = gte_shift(value, sf);
	int value_12 = int(gte_shift(value, 1));
	int max = 0x1000;
	int min = 0x0000;

	if (value_sf < min || value_sf > max)
		C2_FLAG |= (1 << 12);

	if (value_12 > max)
		return max;

	if (value_12 < min)
		return min;

	return value_12;
}

int Sony_PlayStation_GTE::gte_cop2(int op)
{
	int v;
	int lm;
	int cv;
	int mx;
	int h_over_sz3 = 0;

	lm = GTE_LM(gteop(op));
	m_sf = GTE_SF(gteop(op));

	C2_FLAG = 0;

	switch (GTE_FUNCT(gteop(op)))
	{
	case 0x00:
	case 0x01:
		h_over_sz3 = GTE_RotTransPers(0, lm);
		C2_MAC0 = int(F((long long)C2_DQB + ((long long)C2_DQA * h_over_sz3)));
		C2_IR0 = Lm_H(m_mac0, 1);
		return 1;

	case 0x06:
		if (b_PGXP)
		{
			if (g_cfg_pgxpTextureCorrection)
			{
				struct fvec3 {
					float x, y, z;
				};
				static auto subtractVec = [](const fvec3& u, const fvec3& v) -> fvec3 {
					return fvec3{ u.x - v.x, u.y - v.y, u.z - v.z };
					};
				static auto crossProduct = [](const fvec3& u, const fvec3& v) -> fvec3 {
					return fvec3{ u.y * v.z - v.y * u.z, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };
					};
				static auto dotProduct = [](const fvec3& u, const fvec3& v) -> float {
					return u.x * v.x + u.y * v.y + u.z * v.z;
					};
				static auto normalize = [](const fvec3& v) -> fvec3 {
					const float invLen = 1.0f / sqrtf(dotProduct(v, v));
					return fvec3{ v.x * invLen, v.y * invLen, v.z * invLen };
					};

				// treat our PGXP triangle as plane
				const fvec3 v0 = *(fvec3*)&g_FP_SXYZ0;
				const fvec3 v1 = *(fvec3*)&g_FP_SXYZ1;
				const fvec3 v2 = *(fvec3*)&g_FP_SXYZ2;
				const fvec3 normal = normalize(crossProduct(subtractVec(v2, v1), subtractVec(v0, v1)));

				C2_MAC0 = dotProduct(v0, normal) < 0 ? -1 : 1;
			}
			else
			{
				float fSX0 = g_FP_SXYZ0.x;
				float fSY0 = g_FP_SXYZ0.y;

				float fSX1 = g_FP_SXYZ1.x;
				float fSY1 = g_FP_SXYZ1.y;

				float fSX2 = g_FP_SXYZ2.x;
				float fSY2 = g_FP_SXYZ2.y;

				float nclip = (fSX0 * fSY1) + (fSX1 * fSY2) + (fSX2 * fSY0) - (fSX0 * fSY2) - (fSX1 * fSY0) - (fSX2 * fSY1);

				float absNclip = fabs(nclip);

				if ((0.1f < absNclip) && (absNclip < 1.0f))
					nclip += (nclip < 0.0f) ? -1.0f : 1.0f;

				C2_MAC0 = static_cast<int32_t>(nclip);
			}
		}
		else
		{
			C2_MAC0 = int(F((long long)(C2_SX0 * C2_SY1) + (C2_SX1 * C2_SY2) + (C2_SX2 * C2_SY0) - (C2_SX0 * C2_SY2) - (C2_SX1 * C2_SY0) - (C2_SX2 * C2_SY1)));
		}
		C2_FLAG = 0;
		return 1;

	case 0x0c:
		C2_MAC1 = A1((long long)(C2_R22 * C2_IR3) - (C2_R33 * C2_IR2));
		C2_MAC2 = A2((long long)(C2_R33 * C2_IR1) - (C2_R11 * C2_IR3));
		C2_MAC3 = A3((long long)(C2_R11 * C2_IR2) - (C2_R22 * C2_IR1));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		return 1;

	case 0x10:
		C2_MAC1 = A1((C2_R << 16) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - (C2_R << 16)), 0)));
		C2_MAC2 = A2((C2_G << 16) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - (C2_G << 16)), 0)));
		C2_MAC3 = A3((C2_B << 16) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - (C2_B << 16)), 0)));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x11:
		C2_MAC1 = A1((C2_IR1 << 12) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - (C2_IR1 << 12)), 0)));
		C2_MAC2 = A2((C2_IR2 << 12) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - (C2_IR2 << 12)), 0)));
		C2_MAC3 = A3((C2_IR3 << 12) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - (C2_IR3 << 12)), 0)));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x12:
		mx = GTE_MX(gteop(op));
		v = GTE_V(gteop(op));
		cv = GTE_CV(gteop(op));

		switch (cv) {
		case 2:
			C2_MAC1 = A1((long long)(MX12(mx) * VY(v)) + (MX13(mx) * VZ(v)));
			C2_MAC2 = A2((long long)(MX22(mx) * VY(v)) + (MX23(mx) * VZ(v)));
			C2_MAC3 = A3((long long)(MX32(mx) * VY(v)) + (MX33(mx) * VZ(v)));
			Lm_B1(A1(((long long)CV1(cv) << 12) + (MX11(mx) * VX(v))), 0);
			Lm_B2(A2(((long long)CV2(cv) << 12) + (MX21(mx) * VX(v))), 0);
			Lm_B3(A3(((long long)CV3(cv) << 12) + (MX31(mx) * VX(v))), 0);
			break;

		default:
			C2_MAC1 = A1((long long)((long long)CV1(cv) << 12) + (MX11(mx) * VX(v)) + (MX12(mx) * VY(v)) + (MX13(mx) * VZ(v)));
			C2_MAC2 = A2((long long)((long long)CV2(cv) << 12) + (MX21(mx) * VX(v)) + (MX22(mx) * VY(v)) + (MX23(mx) * VZ(v)));
			C2_MAC3 = A3((long long)((long long)CV3(cv) << 12) + (MX31(mx) * VX(v)) + (MX32(mx) * VY(v)) + (MX33(mx) * VZ(v)));
			break;
		}

		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		return 1;

	case 0x13:
		C2_MAC1 = A1((long long)(C2_L11 * C2_VX0) + (C2_L12 * C2_VY0) + (C2_L13 * C2_VZ0));
		C2_MAC2 = A2((long long)(C2_L21 * C2_VX0) + (C2_L22 * C2_VY0) + (C2_L23 * C2_VZ0));
		C2_MAC3 = A3((long long)(C2_L31 * C2_VX0) + (C2_L32 * C2_VY0) + (C2_L33 * C2_VZ0));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
		C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
		C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1(((C2_R << 4) * C2_IR1) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - ((C2_R << 4) * C2_IR1)), 0)));
		C2_MAC2 = A2(((C2_G << 4) * C2_IR2) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - ((C2_G << 4) * C2_IR2)), 0)));
		C2_MAC3 = A3(((C2_B << 4) * C2_IR3) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - ((C2_B << 4) * C2_IR3)), 0)));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x14:
		C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
		C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
		C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1(((C2_R << 4) * C2_IR1) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - ((C2_R << 4) * C2_IR1)), 0)));
		C2_MAC2 = A2(((C2_G << 4) * C2_IR2) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - ((C2_G << 4) * C2_IR2)), 0)));
		C2_MAC3 = A3(((C2_B << 4) * C2_IR3) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - ((C2_B << 4) * C2_IR3)), 0)));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x16:
		for (v = 0; v < 3; v++) {
			C2_MAC1 = A1((long long)(C2_L11 * VX(v)) + (C2_L12 * VY(v)) + (C2_L13 * VZ(v)));
			C2_MAC2 = A2((long long)(C2_L21 * VX(v)) + (C2_L22 * VY(v)) + (C2_L23 * VZ(v)));
			C2_MAC3 = A3((long long)(C2_L31 * VX(v)) + (C2_L32 * VY(v)) + (C2_L33 * VZ(v)));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
			C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
			C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_MAC1 = A1(((C2_R << 4) * C2_IR1) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - ((C2_R << 4) * C2_IR1)), 0)));
			C2_MAC2 = A2(((C2_G << 4) * C2_IR2) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - ((C2_G << 4) * C2_IR2)), 0)));
			C2_MAC3 = A3(((C2_B << 4) * C2_IR3) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - ((C2_B << 4) * C2_IR3)), 0)));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_RGB0 = C2_RGB1;
			C2_RGB1 = C2_RGB2;
			C2_CD2 = C2_CODE;
			C2_R2 = Lm_C1(C2_MAC1 >> 4);
			C2_G2 = Lm_C2(C2_MAC2 >> 4);
			C2_B2 = Lm_C3(C2_MAC3 >> 4);
		}
		return 1;

	case 0x1b:
		C2_MAC1 = A1((long long)(C2_L11 * C2_VX0) + (C2_L12 * C2_VY0) + (C2_L13 * C2_VZ0));
		C2_MAC2 = A2((long long)(C2_L21 * C2_VX0) + (C2_L22 * C2_VY0) + (C2_L23 * C2_VZ0));
		C2_MAC3 = A3((long long)(C2_L31 * C2_VX0) + (C2_L32 * C2_VY0) + (C2_L33 * C2_VZ0));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
		C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
		C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1((C2_R << 4) * C2_IR1);
		C2_MAC2 = A2((C2_G << 4) * C2_IR2);
		C2_MAC3 = A3((C2_B << 4) * C2_IR3);
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x1c:
		C2_MAC1 = A1((long long)(((long long)C2_RBK) << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
		C2_MAC2 = A2((long long)(((long long)C2_GBK) << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
		C2_MAC3 = A3((long long)(((long long)C2_BBK) << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1((C2_R << 4) * C2_IR1);
		C2_MAC2 = A2((C2_G << 4) * C2_IR2);
		C2_MAC3 = A3((C2_B << 4) * C2_IR3);
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x1e:
		C2_MAC1 = A1((long long)(C2_L11 * C2_VX0) + (C2_L12 * C2_VY0) + (C2_L13 * C2_VZ0));
		C2_MAC2 = A2((long long)(C2_L21 * C2_VX0) + (C2_L22 * C2_VY0) + (C2_L23 * C2_VZ0));
		C2_MAC3 = A3((long long)(C2_L31 * C2_VX0) + (C2_L32 * C2_VY0) + (C2_L33 * C2_VZ0));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
		C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
		C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x20:
		for (v = 0; v < 3; v++) {
			C2_MAC1 = A1((long long)(C2_L11 * VX(v)) + (C2_L12 * VY(v)) + (C2_L13 * VZ(v)));
			C2_MAC2 = A2((long long)(C2_L21 * VX(v)) + (C2_L22 * VY(v)) + (C2_L23 * VZ(v)));
			C2_MAC3 = A3((long long)(C2_L31 * VX(v)) + (C2_L32 * VY(v)) + (C2_L33 * VZ(v)));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
			C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
			C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_RGB0 = C2_RGB1;
			C2_RGB1 = C2_RGB2;
			C2_CD2 = C2_CODE;
			C2_R2 = Lm_C1(C2_MAC1 >> 4);
			C2_G2 = Lm_C2(C2_MAC2 >> 4);
			C2_B2 = Lm_C3(C2_MAC3 >> 4);
		}
		return 1;

	case 0x28:
		C2_MAC1 = A1(C2_IR1 * C2_IR1);
		C2_MAC2 = A2(C2_IR2 * C2_IR2);
		C2_MAC3 = A3(C2_IR3 * C2_IR3);
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		return 1;

	case 0x29:
		C2_MAC1 = A1(((C2_R << 4) * C2_IR1) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - ((C2_R << 4) * C2_IR1)), 0)));
		C2_MAC2 = A2(((C2_G << 4) * C2_IR2) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - ((C2_G << 4) * C2_IR2)), 0)));
		C2_MAC3 = A3(((C2_B << 4) * C2_IR3) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - ((C2_B << 4) * C2_IR3)), 0)));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x2a:
		for (v = 0; v < 3; v++) {
			C2_MAC1 = A1((C2_R0 << 16) + (C2_IR0 * Lm_B1(A1(((long long)C2_RFC << 12) - (C2_R0 << 16)), 0)));
			C2_MAC2 = A2((C2_G0 << 16) + (C2_IR0 * Lm_B2(A2(((long long)C2_GFC << 12) - (C2_G0 << 16)), 0)));
			C2_MAC3 = A3((C2_B0 << 16) + (C2_IR0 * Lm_B3(A3(((long long)C2_BFC << 12) - (C2_B0 << 16)), 0)));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_RGB0 = C2_RGB1;
			C2_RGB1 = C2_RGB2;
			C2_CD2 = C2_CODE;
			C2_R2 = Lm_C1(C2_MAC1 >> 4);
			C2_G2 = Lm_C2(C2_MAC2 >> 4);
			C2_B2 = Lm_C3(C2_MAC3 >> 4);
		}
		return 1;

	case 0x2d:
		C2_MAC0 = int(F((long long)(C2_ZSF3 * C2_SZ1) + (C2_ZSF3 * C2_SZ2) + (C2_ZSF3 * C2_SZ3)));
		C2_OTZ = Lm_D(m_mac0, 1);
		return 1;

	case 0x2e:
		C2_MAC0 = int(F((long long)(C2_ZSF4 * C2_SZ0) + (C2_ZSF4 * C2_SZ1) + (C2_ZSF4 * C2_SZ2) + (C2_ZSF4 * C2_SZ3)));
		C2_OTZ = Lm_D(m_mac0, 1);
		return 1;

	case 0x30:
		for (v = 0; v < 3; v++)
			h_over_sz3 = GTE_RotTransPers(v, lm);

		C2_MAC0 = int(F((long long)C2_DQB + ((long long)C2_DQA * h_over_sz3)));
		C2_IR0 = Lm_H(m_mac0, 1);
		return 1;

	case 0x3d:
		C2_MAC1 = A1(C2_IR0 * C2_IR1);
		C2_MAC2 = A2(C2_IR0 * C2_IR2);
		C2_MAC3 = A3(C2_IR0 * C2_IR3);
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x3e:
		C2_MAC1 = A1(gte_shift(C2_MAC1, -m_sf) + (C2_IR0 * C2_IR1));
		C2_MAC2 = A2(gte_shift(C2_MAC2, -m_sf) + (C2_IR0 * C2_IR2));
		C2_MAC3 = A3(gte_shift(C2_MAC3, -m_sf) + (C2_IR0 * C2_IR3));
		C2_IR1 = Lm_B1(C2_MAC1, lm);
		C2_IR2 = Lm_B2(C2_MAC2, lm);
		C2_IR3 = Lm_B3(C2_MAC3, lm);
		C2_RGB0 = C2_RGB1;
		C2_RGB1 = C2_RGB2;
		C2_CD2 = C2_CODE;
		C2_R2 = Lm_C1(C2_MAC1 >> 4);
		C2_G2 = Lm_C2(C2_MAC2 >> 4);
		C2_B2 = Lm_C3(C2_MAC3 >> 4);
		return 1;

	case 0x3f:
		for (v = 0; v < 3; v++) {
			C2_MAC1 = A1((long long)(C2_L11 * VX(v)) + (C2_L12 * VY(v)) + (C2_L13 * VZ(v)));
			C2_MAC2 = A2((long long)(C2_L21 * VX(v)) + (C2_L22 * VY(v)) + (C2_L23 * VZ(v)));
			C2_MAC3 = A3((long long)(C2_L31 * VX(v)) + (C2_L32 * VY(v)) + (C2_L33 * VZ(v)));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_MAC1 = A1((long long)((long long)C2_RBK << 12) + (C2_LR1 * C2_IR1) + (C2_LR2 * C2_IR2) + (C2_LR3 * C2_IR3));
			C2_MAC2 = A2((long long)((long long)C2_GBK << 12) + (C2_LG1 * C2_IR1) + (C2_LG2 * C2_IR2) + (C2_LG3 * C2_IR3));
			C2_MAC3 = A3((long long)((long long)C2_BBK << 12) + (C2_LB1 * C2_IR1) + (C2_LB2 * C2_IR2) + (C2_LB3 * C2_IR3));
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_MAC1 = A1((C2_R << 4) * C2_IR1);
			C2_MAC2 = A2((C2_G << 4) * C2_IR2);
			C2_MAC3 = A3((C2_B << 4) * C2_IR3);
			C2_IR1 = Lm_B1(C2_MAC1, lm);
			C2_IR2 = Lm_B2(C2_MAC2, lm);
			C2_IR3 = Lm_B3(C2_MAC3, lm);
			C2_RGB0 = C2_RGB1;
			C2_RGB1 = C2_RGB2;
			C2_CD2 = C2_CODE;
			C2_R2 = Lm_C1(C2_MAC1 >> 4);
			C2_G2 = Lm_C2(C2_MAC2 >> 4);
			C2_B2 = Lm_C3(C2_MAC3 >> 4);
		}
		return 1;
	}

	return 0;
}


/*
	PSYQ SDK Helper Functions
*/
void Sony_PlayStation_GTE::SetDQA(int iDQA)
{
	CTC2(*(uint32_t*)&iDQA, 27);
}

void Sony_PlayStation_GTE::SetDQB(int iDQB)
{
	CTC2(*(uint32_t*)&iDQB, 28);
}

int Sony_PlayStation_GTE::sin_1(int a)
{
	short sin;

	if (a >= 2048)
	{
		//loc_284
		if (a <= 3072)
		{
			//loc_2B0
			sin = -rsin_tbl[a - 2048];
		}
		else
		{
			sin = -rsin_tbl[4096 - a];
		}
	}
	else if (a >= 1024)
	{
		//loc_264
		sin = rsin_tbl[2048 - a];
	}
	else
	{
		sin = rsin_tbl[a];
	}

	return sin;
}


/*
	Custom
*/
void Sony_PlayStation_GTE::SetIdentity(MATRIX* m)
{
	m->m[0][0] = ONE;	m->m[0][1] = 0;		m->m[0][2] = 0;
	m->m[1][0] = 0;		m->m[1][1] = ONE;	m->m[1][2] = 0;
	m->m[2][0] = 0;		m->m[2][1] = 0;		m->m[2][2] = ONE;
	m->t[0] = 0;		m->t[1] = 0;		m->t[2] = 0;
}
void Sony_PlayStation_GTE::SetIdentity(MATRIX2* m)
{
	m->m00 = ONE;	m->m01 = 0;		m->m02 = 0;
	m->m10 = 0;		m->m11 = ONE;	m->m12 = 0;
	m->m20 = 0;		m->m21 = 0;		m->m22 = ONE;
	m->tx = 0;		m->ty = 0;		m->tz = 0;
}
void Sony_PlayStation_GTE::CompM(MATRIX* m0, MATRIX* m1, MATRIX* m2)
{
	MATRIX* pMVar1;
	MATRIX* r0;
	MATRIX* r0_00;
	VECTOR* r0_01;

	gte_SetRotMatrix((MATRIX*)m0);
	gte_SetTransMatrix((MATRIX*)m0);
	gte_ldclmv((MATRIX*)m1);
	gte_rtir();
	pMVar1 = (MATRIX*)(m1->m[0] + 1);
	r0 = (MATRIX*)(m2->m[0] + 1);
	gte_stclmv((MATRIX*)m2);
	gte_ldclmv(pMVar1);
	gte_rtir();
	r0_00 = (MATRIX*)(m1->m[0] + 2);
	pMVar1 = (MATRIX*)(m2->m[0] + 2);
	gte_stclmv(r0);
	gte_ldclmv(r0_00);
	gte_rtir();
	r0_01 = (VECTOR*)(m1->t + 2);
	gte_stclmv(pMVar1);
	gte_ldlv0(r0_01);
	gte_rt();
	gte_stlvnl((VECTOR*)(m2->t + 2));
}


/*
	PSYQ SDK
*/
void Sony_PlayStation_GTE::InitGeom(void)
{
	C2_ZSF3 = 341;
	C2_ZSF4 = 256;
	C2_H = 1000;
	C2_DQA = -98;
	C2_DQB = 340;
	C2_OFX = 0;
	C2_OFY = 0;
}

MATRIX* Sony_PlayStation_GTE::MulMatrix0(MATRIX* m0, MATRIX* m1, MATRIX* m2)
{
	SVECTOR v0, r0, r1, r2;

	gte_SetRotMatrix(m0);

	v0.vx = m1->m[0][0];
	v0.vy = m1->m[1][0];
	v0.vz = m1->m[2][0];

	gte_ldv0(&v0);
	gte_rtv0();
	gte_stsv(&r0);

	v0.vx = m1->m[0][1];
	v0.vy = m1->m[1][1];
	v0.vz = m1->m[2][1];

	gte_ldv0(&v0);
	gte_rtv0();
	gte_stsv(&r1);

	v0.vx = m1->m[0][2];
	v0.vy = m1->m[1][2];
	v0.vz = m1->m[2][2];

	gte_ldv0(&v0);
	gte_rtv0();
	gte_stsv(&r2);

	m2->m[0][0] = r0.vx;
	m2->m[0][1] = r1.vx;
	m2->m[0][2] = r2.vx;

	m2->m[1][0] = r0.vy;
	m2->m[1][1] = r1.vy;
	m2->m[1][2] = r2.vy;

	m2->m[2][0] = r0.vz;
	m2->m[2][1] = r1.vz;
	m2->m[2][2] = r2.vz;

	return m2;
}

MATRIX* Sony_PlayStation_GTE::MulMatrix(MATRIX* m0, MATRIX* m1)
{
	MATRIX tmp;
	gte_MulMatrix0(m0, m1, &tmp);

	*m0 = tmp;

	return m0;
}

MATRIX* Sony_PlayStation_GTE::MulMatrix2(MATRIX* m0, MATRIX* m1)
{
	// Same as MulMatrix but result goes to m1
	MATRIX tmp;
	gte_MulMatrix0(m0, m1, &tmp);

	*m1 = tmp;

	return m1;
}

MATRIX* Sony_PlayStation_GTE::MulRotMatrix(MATRIX* m0)
{
	// FIXME: might be wrong
	// as RTV0 can be insufficient
	gte_ldv0(&m0->m[0]);
	gte_rtv0();
	gte_stsv(&m0->m[0]);

	gte_ldv0(&m0->m[1]);
	gte_rtv0();
	gte_stsv(&m0->m[1]);

	gte_ldv0(&m0->m[2]);
	gte_rtv0();
	gte_stsv(&m0->m[2]);

	return m0;
}

VECTOR* Sony_PlayStation_GTE::ApplyMatrix(MATRIX* m, SVECTOR* v0, VECTOR* v1)
{
	gte_SetRotMatrix(m);
	gte_ldv0(v0);
	gte_rtv0();
	gte_stlvnl(v1);
	return v1;
}

VECTOR* Sony_PlayStation_GTE::ApplyRotMatrix(SVECTOR* v0, VECTOR* v1)
{
	gte_ldv0(v0);
	gte_rtv0();
	gte_stlvnl(v1);
	return v1;
}

VECTOR* Sony_PlayStation_GTE::ApplyRotMatrixLV(VECTOR* v0, VECTOR* v1)
{
	VECTOR tmpHI;
	VECTOR tmpLO;

	tmpHI.vx = v0->vx;
	tmpHI.vy = v0->vy;
	tmpHI.vz = v0->vz;

	if (tmpHI.vx < 0)
	{
		tmpLO.vx = -(-tmpHI.vx >> 0xf);
		tmpHI.vx = -(-tmpHI.vx & 0x7fff);
	}
	else
	{
		tmpLO.vx = tmpHI.vx >> 0xf;
		tmpHI.vx = tmpHI.vx & 0x7fff;
	}

	if (tmpHI.vy < 0)
	{
		tmpLO.vy = -(-tmpHI.vy >> 0xf);
		tmpHI.vy = -(-tmpHI.vy & 0x7fff);
	}
	else
	{
		tmpLO.vy = tmpHI.vy >> 0xf;
		tmpHI.vy = tmpHI.vy & 0x7fff;
	}

	if (tmpHI.vz < 0)
	{
		tmpLO.vz = -(-tmpHI.vz >> 0xf);
		tmpHI.vz = -(-tmpHI.vz & 0x7fff);
	}
	else
	{
		tmpLO.vz = tmpHI.vz >> 0xf;
		tmpHI.vz = tmpHI.vz & 0x7fff;
	}

	gte_ldlvl(&tmpLO);
	gte_rtir_sf0();
	gte_stlvnl(&tmpLO);

	gte_ldlvl(&tmpHI);
	gte_rtir();

	if (tmpLO.vx < 0)
		tmpLO.vx *= 8;
	else
		tmpLO.vx <<= 3;

	if (tmpLO.vy < 0)
		tmpLO.vy *= 8;
	else
		tmpLO.vy <<= 3;

	if (tmpLO.vz < 0)
		tmpLO.vz *= 8;
	else
		tmpLO.vz <<= 3;

	gte_stlvnl(&tmpHI);

	v1->vx = tmpHI.vx + tmpLO.vx;
	v1->vy = tmpHI.vy + tmpLO.vy;
	v1->vz = tmpHI.vz + tmpLO.vz;

	return v1;
}

VECTOR* Sony_PlayStation_GTE::ApplyMatrixLV(MATRIX* m, VECTOR* v0, VECTOR* v1)
{
	VECTOR tmpHI;
	VECTOR tmpLO;

	gte_SetRotMatrix(m);

	tmpHI.vx = v0->vx;
	tmpHI.vy = v0->vy;
	tmpHI.vz = v0->vz;

	if (tmpHI.vx < 0)
	{
		tmpLO.vx = -(-tmpHI.vx >> 0xf);
		tmpHI.vx = -(-tmpHI.vx & 0x7fff);
	}
	else
	{
		tmpLO.vx = tmpHI.vx >> 0xf;
		tmpHI.vx = tmpHI.vx & 0x7fff;
	}

	if (tmpHI.vy < 0)
	{
		tmpLO.vy = -(-tmpHI.vy >> 0xf);
		tmpHI.vy = -(-tmpHI.vy & 0x7fff);
	}
	else
	{
		tmpLO.vy = tmpHI.vy >> 0xf;
		tmpHI.vy = tmpHI.vy & 0x7fff;
	}

	if (tmpHI.vz < 0)
	{
		tmpLO.vz = -(-tmpHI.vz >> 0xf);
		tmpHI.vz = -(-tmpHI.vz & 0x7fff);
	}
	else
	{
		tmpLO.vz = tmpHI.vz >> 0xf;
		tmpHI.vz = tmpHI.vz & 0x7fff;
	}

	gte_ldlvl(&tmpLO);
	gte_rtir_sf0();
	gte_stlvnl(&tmpLO);

	gte_ldlvl(&tmpHI);
	gte_rtir();

	if (tmpLO.vx < 0)
		tmpLO.vx *= 8;
	else
		tmpLO.vx <<= 3;

	if (tmpLO.vy < 0)
		tmpLO.vy *= 8;
	else
		tmpLO.vy <<= 3;

	if (tmpLO.vz < 0)
		tmpLO.vz *= 8;
	else
		tmpLO.vz <<= 3;

	gte_stlvnl(&tmpHI);

	v1->vx = tmpHI.vx + tmpLO.vx;
	v1->vy = tmpHI.vy + tmpLO.vy;
	v1->vz = tmpHI.vz + tmpLO.vz;
	return v1;
}

SVECTOR* Sony_PlayStation_GTE::ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1)
{
	gte_SetRotMatrix(m);
	gte_ldv0(v0);
	gte_rtv0();
	gte_stsv(v1);
	return v1;
}

MATRIX* Sony_PlayStation_GTE::RotMatrix(SVECTOR* r, MATRIX* m)
{
	// correct Psy-Q implementation
	int c0, c1, c2;
	int s0, s1, s2;
	int s2p0, s2m0, c2p0, c2m0;
	int	s2c0, s2s0, c2c0, c2s0;

	c0 = rcos(r->vx);
	c1 = rcos(r->vy);
	c2 = rcos(r->vz);
	s0 = rsin(r->vx);
	s1 = rsin(r->vy);
	s2 = rsin(r->vz);
	s2p0 = rsin(r->vz + r->vx);
	s2m0 = rsin(r->vz - r->vx);
	c2p0 = rcos(r->vz + r->vx);
	c2m0 = rcos(r->vz - r->vx);
	s2c0 = (s2p0 + s2m0) / 2;
	c2s0 = (s2p0 - s2m0) / 2;
	s2s0 = (c2m0 - c2p0) / 2;
	c2c0 = (c2m0 + c2p0) / 2;

	m->m[0][0] = FIXED(c2 * c1);
	m->m[1][0] = s2c0 + FIXED(c2s0 * s1);
	m->m[2][0] = s2s0 - FIXED(c2c0 * s1);
	m->m[0][1] = -FIXED(s2 * c1);
	m->m[1][1] = c2c0 - FIXED(s2s0 * s1);
	m->m[2][1] = c2s0 + FIXED(s2c0 * s1);
	m->m[0][2] = s1;
	m->m[1][2] = -FIXED(c1 * s0);
	m->m[2][2] = FIXED(c1 * c0);

	return m;
}

MATRIX* Sony_PlayStation_GTE::RotMatrixYXZ(SVECTOR* r, MATRIX* m)
{
	// correct Psy-Q implementation
	int c0, c1, c2;
	int s0, s1, s2;

	c0 = rcos(r->vx);
	c1 = rcos(r->vy);
	c2 = rcos(r->vz);
	s0 = rsin(r->vx);
	s1 = rsin(r->vy);
	s2 = rsin(r->vz);

	// Y-axis
	m->m[1][0] = FIXED(s2 * c0);
	m->m[1][1] = FIXED(c2 * c0);
	m->m[1][2] = -s0;

	// X-axis
	int x0 = FIXED(s1 * s0);
	m->m[0][0] = FIXED(c1 * c2) + FIXED(x0 * s2);
	m->m[0][1] = FIXED(x0 * c2) - FIXED(c1 * s2);
	m->m[0][2] = FIXED(s1 * c0);

	// Z-axis
	int z0 = FIXED(c1 * s0);
	m->m[2][1] = FIXED(s1 * s2) + FIXED(z0 * c2);
	m->m[2][0] = FIXED(z0 * s2) - FIXED(s1 * c2);
	m->m[2][2] = FIXED(c1 * c0);

	return m;
}

MATRIX* Sony_PlayStation_GTE::RotMatrixZYX_gte(SVECTOR* r, MATRIX* m)
{
	m->m[0][0] = 0x1000;
	m->m[0][1] = 0;
	m->m[0][2] = 0;

	m->m[1][0] = 0;
	m->m[1][1] = 0x1000;
	m->m[1][2] = 0;

	m->m[2][0] = 0;
	m->m[2][1] = 0;
	m->m[2][2] = 0x1000;

	RotMatrixX(r->vx, m);
	RotMatrixY(r->vy, m);
	RotMatrixZ(r->vz, m);
	return m;
}

MATRIX* Sony_PlayStation_GTE::RotMatrixX(int32_t r, MATRIX* m)
{
	// correct Psy-Q implementation
	int s0 = rsin(r);
	int c0 = rcos(r);
	int t1, t2;
	t1 = m->m[1][0];
	t2 = m->m[2][0];
	m->m[1][0] = FIXED(t1 * c0 - t2 * s0);
	m->m[2][0] = FIXED(t1 * s0 + t2 * c0);
	t1 = m->m[1][1];
	t2 = m->m[2][1];
	m->m[1][1] = FIXED(t1 * c0 - t2 * s0);
	m->m[2][1] = FIXED(t1 * s0 + t2 * c0);
	t1 = m->m[1][2];
	t2 = m->m[2][2];
	m->m[1][2] = FIXED(t1 * c0 - t2 * s0);
	m->m[2][2] = FIXED(t1 * s0 + t2 * c0);

	return m;
}

MATRIX* Sony_PlayStation_GTE::RotMatrixY(int32_t r, MATRIX* m)
{
	// correct Psy-Q implementation
	int s0 = rsin(r);
	int c0 = rcos(r);
	int t1, t2;
	t1 = m->m[0][0];
	t2 = m->m[2][0];
	m->m[0][0] = FIXED(t1 * c0 + t2 * s0);
	m->m[2][0] = FIXED(-t1 * s0 + t2 * c0);
	t1 = m->m[0][1];
	t2 = m->m[2][1];
	m->m[0][1] = FIXED(t1 * c0 + t2 * s0);
	m->m[2][1] = FIXED(-t1 * s0 + t2 * c0);
	t1 = m->m[0][2];
	t2 = m->m[2][2];
	m->m[0][2] = FIXED(t1 * c0 + t2 * s0);
	m->m[2][2] = FIXED(-t1 * s0 + t2 * c0);

	return m;
}

MATRIX* Sony_PlayStation_GTE::RotMatrixZ(int32_t r, MATRIX* m)
{
	// correct Psy-Q implementation
	int s0 = rsin(r);
	int c0 = rcos(r);
	int t1, t2;
	t1 = m->m[0][0];
	t2 = m->m[1][0];
	m->m[0][0] = FIXED(t1 * c0 - t2 * s0);
	m->m[1][0] = FIXED(t1 * s0 + t2 * c0);
	t1 = m->m[0][1];
	t2 = m->m[1][1];
	m->m[0][1] = FIXED(t1 * c0 - t2 * s0);
	m->m[1][1] = FIXED(t1 * s0 + t2 * c0);
	t1 = m->m[0][2];
	t2 = m->m[1][2];
	m->m[0][2] = FIXED(t1 * c0 - t2 * s0);
	m->m[1][2] = FIXED(t1 * s0 + t2 * c0);

	return m;
}

MATRIX* Sony_PlayStation_GTE::TransMatrix(MATRIX* m, VECTOR* v)
{
	m->t[0] = v->vx;
	m->t[1] = v->vy;
	m->t[2] = v->vz;
	return m;
}

MATRIX* Sony_PlayStation_GTE::ScaleMatrix(MATRIX* m, VECTOR* v)
{
	m->m[0][0] = static_cast<int16_t>(FIXED(m->m[0][0] * v->vx));
	m->m[0][1] = static_cast<int16_t>(FIXED(m->m[0][1] * v->vx));
	m->m[0][2] = static_cast<int16_t>(FIXED(m->m[0][2] * v->vx));
	m->m[1][0] = static_cast<int16_t>(FIXED(m->m[1][0] * v->vy));
	m->m[1][1] = static_cast<int16_t>(FIXED(m->m[1][1] * v->vy));
	m->m[1][2] = static_cast<int16_t>(FIXED(m->m[1][2] * v->vy));
	m->m[2][0] = static_cast<int16_t>(FIXED(m->m[2][0] * v->vz));
	m->m[2][1] = static_cast<int16_t>(FIXED(m->m[2][1] * v->vz));
	m->m[2][2] = static_cast<int16_t>(FIXED(m->m[2][2] * v->vz));
	return m;
}

MATRIX* Sony_PlayStation_GTE::CompMatrix(MATRIX* m0, MATRIX* m1, MATRIX* m2)
{
	gte_MulMatrix0(m0, m1, m2);

	VECTOR tmp { m1->t[0], m1->t[1], m1->t[2], 0 };

	gte_ldlvl(&tmp);
	gte_rtir_sf0();
	gte_stlvnl(m2->t);

	m2->t[0] += m0->t[0];
	m2->t[1] += m0->t[1];
	m2->t[2] += m0->t[2];

	return m2;
}

MATRIX* Sony_PlayStation_GTE::CompMatrixLV(MATRIX* m0, MATRIX* m1, MATRIX* m2)
{
	// UNTESTED
	// correct Psy-Q implementation
	VECTOR tmpHI;
	VECTOR tmpLO;

	gte_MulMatrix0(m0, m1, m2);

	// next... same as ApplyMatrixLV
	tmpHI.vx = m1->t[0];
	tmpHI.vy = m1->t[1];
	tmpHI.vz = m1->t[2];

	if (tmpHI.vx < 0)
	{
		tmpLO.vx = -(-tmpHI.vx >> 0xf);
		tmpHI.vx = -(-tmpHI.vx & 0x7fff);
	}
	else
	{
		tmpLO.vx = tmpHI.vx >> 0xf;
		tmpHI.vx = tmpHI.vx & 0x7fff;
	}

	if (tmpHI.vy < 0)
	{
		tmpLO.vy = -(-tmpHI.vy >> 0xf);
		tmpHI.vy = -(-tmpHI.vy & 0x7fff);
	}
	else
	{
		tmpLO.vy = tmpHI.vy >> 0xf;
		tmpHI.vy = tmpHI.vy & 0x7fff;
	}

	if (tmpHI.vz < 0)
	{
		tmpLO.vz = -(-tmpHI.vz >> 0xf);
		tmpHI.vz = -(-tmpHI.vz & 0x7fff);
	}
	else
	{
		tmpLO.vz = tmpHI.vz >> 0xf;
		tmpHI.vz = tmpHI.vz & 0x7fff;
	}

	gte_ldlvl(&tmpLO);
	gte_rtir_sf0();
	gte_stlvnl(&tmpLO);

	gte_ldlvl(&tmpHI);
	gte_rtir();

	if (tmpLO.vx < 0)
		tmpLO.vx = tmpLO.vx * 8;
	else
		tmpLO.vx = tmpLO.vx << 3;

	if (tmpLO.vy < 0)
		tmpLO.vy = tmpLO.vy * 8;
	else
		tmpLO.vy = tmpLO.vy << 3;

	if (tmpLO.vz < 0)
		tmpLO.vz = tmpLO.vz * 8;
	else
		tmpLO.vz = tmpLO.vz << 3;

	gte_stlvnl(&tmpHI);

	m2->t[0] = tmpHI.vx + tmpLO.vx + m0->t[0];
	m2->t[1] = tmpHI.vy + tmpLO.vy + m0->t[1];
	m2->t[2] = tmpHI.vz + tmpLO.vz + m0->t[2];

	return m2;
}

void Sony_PlayStation_GTE::SetRotMatrix(MATRIX* m)
{
	gte_SetRotMatrix(m);
}

void Sony_PlayStation_GTE::SetLightMatrix(MATRIX* m)
{
	gte_SetLightMatrix(m);
}

void Sony_PlayStation_GTE::SetColorMatrix(MATRIX* m)
{
	gte_SetColorMatrix(m);
}

void Sony_PlayStation_GTE::SetTransMatrix(MATRIX* m)
{
	gte_SetTransMatrix(m);
}

void Sony_PlayStation_GTE::PushMatrix(void)
{
	if (matrixLevel < 20)
	{
		MATRIX* m = &stack[matrixLevel];//$t7

		gte_ReadRotMatrix(m);
		gte_sttr(m->t);

		currentMatrix = std::make_shared<MATRIX>(stack[matrixLevel++]);
	}
	else
	{
		std::cout << "Error: Can't push matrix,stack(max 20) is full!" << std::endl;
	}
}

void Sony_PlayStation_GTE::PopMatrix(void)
{
	if (matrixLevel > 0)
	{
		currentMatrix = std::make_shared<MATRIX>(stack[matrixLevel--]);
		MATRIX* m = &stack[matrixLevel];//$t7

		gte_SetRotMatrix(m);
		gte_SetTransMatrix(m);
	}
	else
	{
		std::cout << "Error: Can't pop matrix,stack is empty!" << std::endl;
	}
}

void Sony_PlayStation_GTE::ReadRotMatrix(MATRIX* m)
{
	gte_ReadRotMatrix(m);
}

void Sony_PlayStation_GTE::ReadLightMatrix(MATRIX* m)
{
	gte_ReadLightMatrix(m);
}

void Sony_PlayStation_GTE::ReadColorMatrix(MATRIX* m)
{
	gte_ReadColorMatrix(m);
}

void Sony_PlayStation_GTE::SetRGBcd(CVECTOR* v)
{
	gte_ldrgb(v);
}

void Sony_PlayStation_GTE::SetBackColor(long rbk, long gbk, long bbk)
{
	gte_SetBackColor(rbk, gbk, bbk);
}

void Sony_PlayStation_GTE::SetFarColor(long rfc, long gfc, long bfc)
{
	gte_SetFarColor(rfc, gfc, bfc);
}

void Sony_PlayStation_GTE::SetGeomOffset(long ofx, long ofy)
{
	C2_OFX = (ofx << 16);
	C2_OFY = (ofy << 16);
}

void Sony_PlayStation_GTE::SetGeomScreen(long h)
{
	C2_H = h;
}

void Sony_PlayStation_GTE::ReadSZfifo3(long* sz0, long* sz1, long* sz2)
{
	gte_stsz3(sz0, sz1, sz2);
}

void Sony_PlayStation_GTE::ReadSZfifo4(long* szx, long* sz0, long* sz1, long* sz2)
{
	gte_stsz4(szx, sz0, sz1, sz2);
}

void Sony_PlayStation_GTE::ReadSXSYfifo(long* sxy0, long* sxy1, long* sxy2)
{
	gte_stsxy3(sxy0, sxy1, sxy2);
}

void Sony_PlayStation_GTE::ReadRGBfifo(CVECTOR* v0, CVECTOR* v1, CVECTOR* v2)
{
	gte_strgb3(v0, v1, v2);
}

void Sony_PlayStation_GTE::ReadGeomOffset(long* ofx, long* ofy)
{
	long TmpX = C2_OFX;
	long TmpY = C2_OFY;
	*ofx = (TmpX >> 16);
	*ofy = (TmpY >> 16);
}

long Sony_PlayStation_GTE::ReadGeomScreen(void)
{
	return C2_H;
}

long Sony_PlayStation_GTE::RotTransPers(SVECTOR* v0, long* sxy, long* p, long* flag)
{
	long sz;
	gte_RotTransPers(v0, sxy, p, flag, &sz);
	return sz;
}

long Sony_PlayStation_GTE::RotTransPers3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag)
{
	long sz;
	gte_RotTransPers3(v0, v1, v2, sxy0, sxy1, sxy2, p, flag, &sz);
	return sz;
}

void Sony_PlayStation_GTE::RotTrans(SVECTOR* v0, VECTOR* v1, long* flag)
{
	gte_RotTrans(v0, v1, flag);
}

void Sony_PlayStation_GTE::RotTransSV(SVECTOR* v0, SVECTOR* v1, long* flag)
{
	gte_ldv0(v0);
	gte_rt();
	gte_stsv(v1);
	gte_stflg(flag);
}

void Sony_PlayStation_GTE::LocalLight(SVECTOR* v0, VECTOR* v1)
{
	gte_LocalLight(v0, v1);
}

void Sony_PlayStation_GTE::LightColor(VECTOR* v0, VECTOR* v1)
{
	gte_LightColor(v0, v1);
}

void Sony_PlayStation_GTE::DpqColorLight(VECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2)
{
	gte_DpqColorLight(v0, v1, p, v2);
}

void Sony_PlayStation_GTE::DpqColor(CVECTOR* v0, long p, CVECTOR* v1)
{
	gte_DpqColor(v0, &p, v1);
}

void Sony_PlayStation_GTE::DpqColor3(CVECTOR* v0, CVECTOR* v1, CVECTOR* v2, long p, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5)
{
	gte_DpqColor3(v0, v1, v2, p, v3, v4, v5);
}

void Sony_PlayStation_GTE::Intpl(VECTOR* v0, long p, CVECTOR* v1)
{
	gte_Intpl(v0, p, v1);
}

VECTOR* Sony_PlayStation_GTE::Square12(VECTOR* v0, VECTOR* v1)
{
	gte_Square12(v0, v1);
	return v1;
}

VECTOR* Sony_PlayStation_GTE::Square0(VECTOR* v0, VECTOR* v1)
{
	gte_Square0(v0, v1);
	return v1;
}

VECTOR* Sony_PlayStation_GTE::SquareSL12(SVECTOR* v0, VECTOR* v1)
{
	gte_ldv0(v0);
	gte_sqr12_b();
	gte_stlvl(v1);
	return v1;
}

VECTOR* Sony_PlayStation_GTE::SquareSL0(SVECTOR* v0, VECTOR* v1)
{
	gte_ldv0(v0);
	gte_sqr0_b();
	gte_stlvl(v1);
	return v1;
}

SVECTOR* Sony_PlayStation_GTE::SquareSS12(SVECTOR* v0, SVECTOR* v1)
{
	gte_ldv0(v0);
	gte_sqr12_b();
	v1->vx = C2_IR1;
	v1->vy = C2_IR2;
	v1->vz = C2_IR3;
	return v1;
}

SVECTOR* Sony_PlayStation_GTE::SquareSS0(SVECTOR* v0, SVECTOR* v1)
{
	gte_ldv0(v0);
	gte_sqr0_b();
	v1->vx = C2_IR1;
	v1->vy = C2_IR2;
	v1->vz = C2_IR3;
	return v1;
}

void Sony_PlayStation_GTE::NormalColor(SVECTOR* v0, CVECTOR* v1)
{
	gte_NormalColor(v0, v1);
}

void Sony_PlayStation_GTE::NormalColor3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5)
{
	gte_NormalColor3(v0, v1, v2, v3, v4, v5);
}

void Sony_PlayStation_GTE::NormalColorDpq(SVECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2)
{
	gte_NormalColorDpq(v0, v1, p, v2);
}

void Sony_PlayStation_GTE::NormalColorDpq3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, long p, CVECTOR* v4, CVECTOR* v5, CVECTOR* v6)
{
	gte_NormalColorDpq3(v0, v1, v2, v3, p, v4, v5, v6);
}

void Sony_PlayStation_GTE::NormalColorCol(SVECTOR* v0, CVECTOR* v1, CVECTOR* v2)
{
	gte_NormalColorCol(v0, v1, v2);
}

void Sony_PlayStation_GTE::NormalColorCol3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5, CVECTOR* v6)
{
	gte_NormalColorCol3(v0, v1, v2, v3, v4, v5, v6);
}

void Sony_PlayStation_GTE::ColorDpq(VECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2)
{
	gte_ColorDpq(v0, v1, p, v2);
}

void Sony_PlayStation_GTE::ColorCol(VECTOR* v0, CVECTOR* v1, CVECTOR* v2)
{
	gte_ColorCol(v0, v1, v2);
}

long Sony_PlayStation_GTE::NormalClip(long sxy0, long sxy1, long sxy2)
{
	long opz;
	gte_NormalClip(&sxy0, &sxy1, &sxy2, &opz);
	return opz;
}

long Sony_PlayStation_GTE::AverageZ3(long sz0, long sz1, long sz2)
{
	long sz;
	gte_AverageZ3(sz0, sz1, sz2, &sz);
	return sz;
}

long Sony_PlayStation_GTE::AverageZ4(long sz0, long sz1, long sz2, long sz3)
{
	long sz;
	gte_AverageZ4(sz0, sz1, sz2, sz3, &sz);
	return sz;
}

void Sony_PlayStation_GTE::OuterProduct12(VECTOR* v0, VECTOR* v1, VECTOR* v2)
{
	gte_OuterProduct12(v0, v1, v2);
}

void Sony_PlayStation_GTE::OuterProduct0(VECTOR* v0, VECTOR* v1, VECTOR* v2)
{
	gte_OuterProduct0(v0, v1, v2);
}

long Sony_PlayStation_GTE::Lzc(long data)
{
	long lzcs;
	gte_Lzc(data, &lzcs);
	return lzcs;
}

long Sony_PlayStation_GTE::RotTransPers4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* flag)
{
	int _flag;
	long sz;

	gte_ldv3(v0, v1, v2);
	gte_rtpt();

	gte_stsxy3(sxy0, sxy1, sxy2);

	gte_stflg(&_flag);

	gte_ldv0(v3);
	gte_rtps();

	gte_stsxy(sxy3);
	gte_stflg(flag);
	gte_stdp(p);

	*flag |= _flag;
	gte_stszotz(&sz);

	return sz;
}

void Sony_PlayStation_GTE::RotTransPersN(SVECTOR* v0, DVECTOR* v1, uint16_t* sz, uint16_t* p, uint16_t* flag, long n)
{
	do {
		gte_ldv0(v0);
		gte_rtps_b();
		v0 = v0 + 1;
		n = n + -1;
		gte_stsxy(v1);
		*sz = (uint16_t)C2_SZ3;
		*p = (uint16_t)C2_IR0;
		*flag = (uint16_t)(C2_FLAG >> 12);
		v1 = v1 + 1;
		sz = sz + 1;
		p = p + 1;
		flag = flag + 1;
	} while (0 < n);
}

long Sony_PlayStation_GTE::RotAverage3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag)
{
	uint32_t Flg0;
	gte_ldv3(v0, v1, v2);
	gte_rtpt_b();
	gte_stsxy3(sxy0, sxy1, sxy2);
	Flg0 = C2_FLAG;
	gte_stdp(p);
	*flag = Flg0;
	gte_avsz3_b();
	return C2_OTZ;
}

long Sony_PlayStation_GTE::RotAverage4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* flag)
{
	uint32_t Flg0;
	uint32_t Flg1;
	gte_ldv3(v0, v1, v2);
	gte_rtpt_b();
	gte_stsxy3(sxy0, sxy1, sxy2);
	Flg0 = C2_FLAG;
	gte_ldv0(v3);
	gte_rtps_b();
	gte_stsxy(sxy3);
	Flg1 = C2_FLAG;
	gte_stdp(p);
	*flag = Flg1 | Flg0;
	gte_avsz4_b();
	return C2_OTZ;
}

long Sony_PlayStation_GTE::RotNclip3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* otz, long* flag)
{
	int opz;
	gte_RotNclip3(v0, v1, v2, sxy0, sxy1, sxy2, p, otz, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotNclip4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* otz, long* flag)
{
	uint32_t Flag;
	gte_ldv3(v0, v1, v2);
	gte_rtpt_b();
	Flag = C2_FLAG;
	gte_nclip_b();
	*flag = Flag;
	if (0 < C2_MAC0) {
		gte_stsxy3(sxy0, sxy1, sxy2);
		gte_ldv0(v3);
		gte_rtps_b();
		gte_stsxy(sxy3);
		gte_stdp(p);
		*otz = C2_SZ3 >> 2;
		C2_FLAG |= Flag;
		*flag = C2_FLAG;
		return C2_FLAG;
	}
	return C2_MAC0;
}

long Sony_PlayStation_GTE::RotAverageNclip3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* otz, long* flag)
{
	int opz;
	gte_RotAverageNclip3(v0, v1, v2, sxy0, sxy1, sxy2, p, otz, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotAverageNclip4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* otz, long* flag)
{
	gte_ldv3(v0, v1, v2);
	gte_rtpt();
	gte_stflg(flag);

	gte_nclip();

	int opz;
	gte_stopz(&opz);

	if (opz > 0)
	{
		gte_stsxy3(sxy0, sxy1, sxy2);

		gte_ldv0(v3);

		gte_rtps();

		gte_stsxy(sxy3);
		gte_stdp(p);
		gte_stflg(flag);

		gte_avsz4();

		gte_stotz(otz);
	}

	return opz;
}

long Sony_PlayStation_GTE::RotColorDpq(SVECTOR* v0, SVECTOR* v1, CVECTOR* v2, long* sxy, CVECTOR* v3, long* flag)
{
	long opz;
	gte_RotColorDpq(v0, v1, v2, sxy, v3, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotColorDpq3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* flag)
{
	long opz;
	gte_RotColorDpq3(v0, v1, v2, v3, v4, v5, v6, sxy0, sxy1, sxy2, v7, v8, v9, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotAverageNclipColorDpq3(
	SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* otz, long* flag)
{
	long opz;
	gte_RotAverageNclipColorDpq3(v0, v1, v2, v3, v4, v5, v6, sxy0, sxy1, sxy2, v7, v8, v9, otz, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotAverageNclipColorCol3(
	SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* otz, long* flag)
{
	long opz;
	gte_RotAverageNclipColorCol3(v0, v1, v2, v3, v4, v5, v6, sxy0, sxy1, sxy2, v7, v8, v9, otz, flag, &opz);
	return opz;
}

long Sony_PlayStation_GTE::RotColorMatDpq(SVECTOR* v0, SVECTOR* v1, CVECTOR* v2, long* sxy, CVECTOR* v3, long matc, long flag)
{
	long sz3;
	gte_ldv0(v0);
	gte_rtps_b();
	flag = C2_FLAG;
	gte_ldv0(v1);
	gte_ll_b();
	do {
		gte_sqr12_b();
		matc--;
	} while (0 < matc);
	gte_ldrgb(v2);
	gte_cdp_b();
	gte_stsxy(sxy);
	sz3 = C2_SZ3;
	gte_strgb(v3);
	return sz3 >> 2;
}

void Sony_PlayStation_GTE::ColorMatDpq(SVECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2, long matc)
{
	gte_ldv0(v0);
	gte_ll_b();
	do {
		gte_sqr12_b();
		matc--;
	} while (0 < matc);
	gte_ldrgb(v1);
	gte_ldIR0(p);
	gte_cdp_b();
	gte_strgb(v2);
}

void Sony_PlayStation_GTE::ColorMatCol(SVECTOR* v0, CVECTOR* v1, CVECTOR* v2, long matc)
{
	gte_ldv0(v0);
	gte_ll_b();
	do {
		gte_sqr12_b();
		matc = matc + -1;
	} while (0 < matc);
	gte_ldrgb(v1);
	gte_cc_b();
	gte_strgb(v2);
}

void Sony_PlayStation_GTE::LoadAverage12(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* v2)
{
	gte_LoadAverage12(v0, v1, p0, p1, v2);
}

void Sony_PlayStation_GTE::LoadAverageShort12(SVECTOR* v0, SVECTOR* v1, long p0, long p1, SVECTOR* v2)
{
	gte_LoadAverageShort12(v0, v1, p0, p1, v2);
}

void Sony_PlayStation_GTE::LoadAverage0(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* v2)
{
	gte_LoadAverage0(v0, v1, p0, p1, v2);
}

void Sony_PlayStation_GTE::LoadAverageShort0(SVECTOR* v0, SVECTOR* v1, long p0, long p1, SVECTOR* v2)
{
	gte_LoadAverageShort0(v0, v1, p0, p1, v2);
}

void Sony_PlayStation_GTE::LoadAverageByte(uint8_t* v0, uint8_t* v1, long p0, long p1, uint8_t* v2)
{
	gte_LoadAverageByte(v0, v1, p0, p1, v2);
}

void Sony_PlayStation_GTE::LoadAverageCol(uint8_t* v0, uint8_t* v1, long p0, long p1, uint8_t* v2)
{
	gte_LoadAverageCol(v0, v1, p0, p1, v2);
}

long Sony_PlayStation_GTE::SquareRoot0(long a)
{
	// correct Psy-Q implementation
	int idx;
	int lzcs;
	lzcs = gte_leadingzerocount(a);

	if (lzcs == 32)
		return 0;

	lzcs &= 0xfffffffe;

	if ((lzcs - 24) < 0)
		idx = a >> (24 - lzcs);
	else
		idx = a << (lzcs - 24);

	return SQRT[idx - 64] << (31 - lzcs >> 1) >> 12;
}

long Sony_PlayStation_GTE::SquareRoot12(long a)
{
	int idx;
	int lzcs;
	lzcs = gte_leadingzerocount(a);

	if (lzcs == 32)
		return 0;

	lzcs &= 0xfffffffe;

	int32_t Shift = (19 - lzcs) >> 1;

	if ((lzcs - 24) < 0)
	{
		idx = a >> (24 - lzcs);
	}
	else
	{
		idx = a << (lzcs - 24);
	}
	if (-1 < Shift)
	{
		return SQRT[idx - 64] << Shift;
	}
	return SQRT[idx - 64] >> -Shift;
}

void Sony_PlayStation_GTE::InvSquareRoot(long a, long* b, long* c)
{
	int idx;
	int lzcs;
	lzcs = gte_leadingzerocount(a);

	if ((lzcs != 32) && (lzcs != 0))
	{
		lzcs = lzcs & 0xfffffffe;
		if ((lzcs - 24) < 0)
		{
			idx = a >> (24 - lzcs);
		}
		else
		{
			idx = a << (lzcs - 24);
		}
		*c = (31 - lzcs) >> 1;
		*b = SQRT_INV[idx - 64];
	}
}

void Sony_PlayStation_GTE::SetFogFar(long a, long h)
{
	assert(h != 0);
	assert((h != -1) && (a * -64 != -0x8000));

	SetDQA((a * -64) / h);
	SetDQB(0x1400000);
}

void Sony_PlayStation_GTE::SetFogNear(long a, long h)
{
	//Error division by 0
	assert(h != 0);
	int depthQ = -(((a << 2) + a) << 6);
	assert(h != -1 && depthQ != 0x8000);
	SetDQA(depthQ / h);
	SetDQB(20971520);
}

void Sony_PlayStation_GTE::SetFogNearFar(long a, long b, long h)
{
	if (b - a < 100)
		return;

	assert((b - a));
	assert((b - a) != -1 && (-a * b) != 32768);
	assert((b - a));
	assert((b - a) != -1 && (b << 12) != 32768);
	assert(h != 0);
	assert(h != -1 && (((-a * b) / (b - a)) << 8) != 32768);

	int dqa = (-a * b / (b - a) << 8) / h;

	SetDQA(MAX(MIN(dqa, 32767), -32767));
	SetDQB((b << 12) / (b - a) << 12);
}

int Sony_PlayStation_GTE::rsin(int a)
{
	if (a >= 0)
	{
		return sin_1(a & 0xFFF);
	}
	else
	{
		//loc_21C
		return -sin_1(-a & 0xFFF);
	}
}

int Sony_PlayStation_GTE::rcos(int a)
{
	short cos;

	if (a < 0)
	{
		a = -a;
	}

	a &= 0xFFF;

	if (a >= 2048)
	{
		//loc_258
		if (a <= 3072)
		{
			cos = -rsin_tbl[3072 - a];
		}
		else
		{
			//loc_27C
			cos = rsin_tbl[a - 3072];
		}
	}
	else if (a >= 1024)
	{
		//loc_240
		cos = -rsin_tbl[a - 1024];
	}
	else
	{
		cos = rsin_tbl[1024 - a];
	}

	return cos;
}

long Sony_PlayStation_GTE::ratan2(long y, long x)
{
	// correct Psy-Q implementation
	int v;
	uint32_t ang;
	int xlt0, ylt0;

	xlt0 = x < 0;
	ylt0 = y < 0;

	if (x == 0 && y == 0)
		return 0;

	if (x < 0)
		x = -x;

	if (y < 0)
		y = -y;

	if (y < x)
	{
		if (((uint32_t)y & 0x7fe00000U) == 0)
			ang = (y << 10) / x;
		else
			ang = y / (x >> 10);

		v = ratan_tbl[ang];
	}
	else
	{
		if (((uint32_t)x & 0x7fe00000U) == 0)
			ang = (x << 10) / y;
		else
			ang = x / (y >> 10);

		v = 1024 - ratan_tbl[ang];
	}

	if (xlt0)
		v = 2048 - v;

	if (ylt0)
		v = -v;

	return v;
}