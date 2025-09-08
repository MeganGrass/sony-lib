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

#pragma once

#include <vector>

#include <memory>


#pragma pack(push, 1)

#include "psyq/libgte.h"

#include "psyq/libgpu.h"

#include "psyq/libgs.h"

#include "half_float.h"


struct MATRIX2
{
    std::int16_t
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22;
    std::uint16_t pad;
    std::int32_t
        tx, ty, tz;
};


struct VECTOR2
{
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
};


struct SVECTOR2
{
    std::int16_t x;
    std::int16_t y;
    std::int16_t z;
};


struct SIZEVECTOR
{
	std::int32_t w;
	std::int32_t h;
	std::int32_t d;
};


struct SHAPEVECTOR
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t w;
	std::int32_t h;
	std::int32_t d;
};


struct MATVECTOR
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t rx;
	std::int32_t ry;
	std::int32_t rz;
	std::int32_t sx;
	std::int32_t sy;
	std::int32_t sz;
};

struct CVECTOR2
{
    uint8_t	r, g, b;
};


struct DVECTORF
{
	half vx{}, vy{};
	uint16_t pgxp_index{};
};


struct PGXPVector3D
{
	float px{}, py{}, pz{};
	half x{}, y{}, z{};
};


struct PGXPVData
{
	uint32_t lookup{};
	float px{}, py{}, pz{};
	float scr_h{}, ofx{}, ofy{};
};


union PSX_PAIR
{
    struct { uint8_t l, h, h2, h3; } b;
    struct { uint16_t l, h; } w;
    struct { int8_t l, h, h2, h3; } sb;
    struct { int16_t l, h; } sw;
    uint32_t d;
    int32_t sd;
};

#pragma pack(pop)

#define MATRIX_IDENTITY { \
    ONE, 0,   0, \
    0,   ONE, 0, \
    0,   0,   ONE, \
    0,   0,   0 \
}

#define MATRIX_SET_IDENTITY(m) \
        (m)->m[0][0] = ONE; (m)->m[0][1] = 0;  (m)->m[0][2] = 0; \
        (m)->m[1][0] = 0; (m)->m[1][1] = ONE; (m)->m[1][2] = 0; \
        (m)->m[2][0] = 0;  (m)->m[2][1] = 0; (m)->m[2][2] = ONE; \
        (m)->t[0] = 0; (m)->t[1] = 0; (m)->t[2] = 0;

#define MATRIX2_SET_IDENTITY(m) \
        (m)->m00 = ONE; (m)->m01 = 0; (m)->m02 = 0; \
        (m)->m10 = 0;  (m)->m11 = ONE; (m)->m12 = 0; \
        (m)->m20 = 0;  (m)->m21 = 0;  (m)->m22 = ONE; \
        (m)->tx = 0;   (m)->ty = 0;   (m)->tz = 0;

#define PGXP_LOOKUP_VALUE(x, y) (*(uint16_t*)&(x) | (*(uint16_t*)&(y) << 16))


class Sony_PlayStation_GTE
{
private:

#pragma pack(push, 1)

    /*
    *	Coprocessor0 (cop0 r0-31)
    */
    union CP0
    {
        struct {
            uint32_t
                Index, Random, EntryLo0, BPC,
                Context, BDA, PIDMask, DCIC,
                BadVAddr, BDAM, EntryHi, BPCM,
                Status, Cause, EPC, PRid,
                Config, LLAddr, WatchLO, WatchHI,
                XContext, Reserved1, Reserved2, Reserved3,
                Reserved4, Reserved5, ECC, CacheErr,
                TagLo, TagHi, ErrorEPC, Reserved6;
        } n;
        uint32_t r[32];
    } CP0;


    /*
    *	Data Register (cop2 r0-31)
    */
    union CP2D
    {
        struct {
            SVECTOR		v0, v1, v2;					// Vector 0, 1, 2
            CVECTOR		rgb;						// Color/code value
            int32_t		otz;						// Average Z value (for Ordering Table)
            int32_t		ir0, ir1, ir2, ir3;			// 16bit Accumulator (IR0 Interpolate, IR1-IR3 Vector)
            DVECTOR     sxy0, sxy1, sxy2, sxyp;		// Screen XY-coordinate FIFO (3 stages)
            int32_t		sz0, sz1, sz2, sz3;			// Screen Z-coordinate FIFO (4 stages)
            CVECTOR		rgb0, rgb1, rgb2;			// Color CRGB-code/color FIFO (3 stages)
            int32_t		reserved;					// Not used
            int32_t		mac0, mac1, mac2, mac3;		// 32bit Accumulator (MAC0 Value, MAC1-MAC3 Vector)
            uint32_t	irgb, orgb;					// Convert RGB Color (48bit vs 15bit)
            int32_t		lzcs, lzcr;					// Count Leading-Zeroes/Ones (sign bits)
        } n;
        uint32_t r[32];
        PSX_PAIR p[32];
    } CP2D;


    /*
    *	Control Register (cop2 r32-63)
    */
    union CP2C
    {
        struct {
            MATRIX2		Rotation;					// Rotation Matrix (3x3), Translation Vector (X,Y,Z)
            MATRIX2		Light;						// Light Source Matrix (3x3), Background Color (R,G,B)
            MATRIX2		Color;						// Light Color Matrix (3x3), Far color (R,G,B)
            int32_t		ofx, ofy;					// Screen Offset (X,Y)
            int32_t		h;							// Projection Plane Distance
            int32_t		dqa, dqb;					// Depth Queing Parameter (A coeff, B offset)
            int32_t		zsf3, zsf4;					// Average Z Scale Factors
            int32_t		flag;						// Returns any calculation errors
        } n;
        uint32_t r[32];
        PSX_PAIR p[32];
    } CP2C;

#pragma pack(pop)

    static int m_sf;
    static long long m_mac0;
    static long long m_mac3;

    /*
        Precision Geometry Transform Pipeline
    */
    bool b_PGXP;
    bool g_cfg_pgxpZBuffer;
    bool g_cfg_pgxpTextureCorrection;
    PGXPVector3D g_FP_SXYZ0;
    PGXPVector3D g_FP_SXYZ1;
    PGXPVector3D g_FP_SXYZ2;
	std::vector<PGXPVData> g_pgxpCache;
    uint16_t g_pgxpVertexIndex;
    int g_pgxpTransformed;
    float g_pgxpZOffset;
    float g_pgxpZScale;
    void PGXP_ClearCache(void);
    uint16_t PGXP_GetIndex(int checkTransform);
    uint16_t PGXP_EmitCacheData(PGXPVData* newData);
    void PGXP_SetZOffsetScale(float offset, float scale);
    int PGXP_GetCacheData(PGXPVData* out, uint32_t lookup, uint16_t indexhint) const;

    /*
        PCSX Emulator
    */
    unsigned int gte_leadingzerocount(unsigned int lzcs);
    inline long long gte_shift(long long a, int sf);
    unsigned int gte_divide(unsigned short numerator, unsigned short denominator);
    int GTE_RotTransPers(int idx, int lm);
    int LIM(int value, int max, int min, unsigned int flag);
    int BOUNDS(long long value, int max_flag, int min_flag);
    unsigned int MFC2(int reg);
    unsigned int CFC2(int reg);
    int MFC2_S(int reg);
    int CFC2_S(int reg);
    void MTC2(unsigned int value, int reg);
    void CTC2(unsigned int value, int reg);
    void MTC2_S(int value, int reg);
    void CTC2_S(int value, int reg);
    int A1(long long a);
    int A2(long long a);
    int A3(long long a);
    int Lm_B1(int a, int lm);
    int Lm_B2(int a, int lm);
    int Lm_B3(int a, int lm);
    int Lm_B3_sf(long long value, int sf, int lm);
    int Lm_C1(int a);
    int Lm_C2(int a);
    int Lm_C3(int a);
    int Lm_D(long long a, int sf);
    unsigned int Lm_E(unsigned int result);
    long long F(long long a);
    int Lm_G1(long long a);
    int Lm_G2(long long a);
    int Lm_G1_ia(long long a);
    int Lm_G2_ia(long long a);
    int Lm_H(long long value, int sf);
    int gte_cop2(int op);

    /*
        PSYQ SDK Helper Functions
    */
    void SetDQA(int iDQA);
    void SetDQB(int iDQB);
    int sin_1(int a);

public:

	int matrixLevel;
	std::vector<MATRIX> stack;
	std::shared_ptr<MATRIX> currentMatrix;

    /*
        Construction
    */
    explicit Sony_PlayStation_GTE(void) :
        CP0(),
        CP2D(),
        CP2C(),
        matrixLevel(0),
        stack(),
        currentMatrix(nullptr),
        b_PGXP(false),
        g_cfg_pgxpZBuffer(false),
        g_cfg_pgxpTextureCorrection(false),
        g_FP_SXYZ0(),
        g_FP_SXYZ1(),
        g_FP_SXYZ2(),
        g_pgxpCache(),
        g_pgxpVertexIndex(0),
        g_pgxpTransformed(0),
        g_pgxpZOffset(0.0f),
        g_pgxpZScale(1.0f)
    {
        stack.resize(20);
        currentMatrix = std::make_shared<MATRIX>(stack[matrixLevel]);
		g_pgxpCache.resize(65536);
        InitGeom();
    }
    virtual ~Sony_PlayStation_GTE(void)
    {
        std::memset(&CP0, 0, sizeof(CP0));
        std::memset(&CP2D, 0, sizeof(CP2D));
        std::memset(&CP2C, 0, sizeof(CP2C));
        currentMatrix.reset();
        stack.clear();
		stack.shrink_to_fit();
		g_pgxpCache.clear();
		g_pgxpCache.shrink_to_fit();
    }

	/*
		Half-precision floating-point from signed 16-bit integer
	*/
	float ToFloat(const short x)
	{
		half h = x;
		return (float)(h) / ONE;
	}

	/*
		Half-precision floating-point from unsigned 16-bit integer
	*/
	float ToFloat(const unsigned short x)
	{
		return (float)(x) / ONE;
	}

	/*
		Floating-point from signed 32-bit integer
	*/
	float ToFloat(const int x)
	{
		return (float)(x) / ONE;
	}

	/*
		Signed 16-bit integer from half-precision floating-point
	*/
	short ToShort(const float x)
	{
		half h = x;
		return (short)h;
	}

	/*
		Custom
	*/
	void SetIdentity(MATRIX* m);
	void SetIdentity(MATRIX2* m);
	void CompM(MATRIX* m0, MATRIX* m1, MATRIX* m2);

    /*
        PSYQ SDK
    */
    void InitGeom(void);

    //void EigenMatrix(MATRIX* m, MATRIX* t);
    //int IsIdMatrix(MATRIX* m);
    MATRIX* MulMatrix0(MATRIX* m0, MATRIX* m1, MATRIX* m2);
    //MATRIX* MulRotMatrix0(MATRIX* m0, MATRIX* m1);
    MATRIX* MulMatrix(MATRIX* m0, MATRIX* m1);
    MATRIX* MulMatrix2(MATRIX* m0, MATRIX* m1);
    MATRIX* MulRotMatrix(MATRIX* m0);
    //MATRIX* SetMulMatrix(MATRIX* m0, MATRIX* m1);
    //MATRIX* SetMulRotMatrix(MATRIX* m0);
    VECTOR* ApplyMatrix(MATRIX* m, SVECTOR* v0, VECTOR* v1);
    VECTOR* ApplyRotMatrix(SVECTOR* v0, VECTOR* v1);
    VECTOR* ApplyRotMatrixLV(VECTOR* v0, VECTOR* v1);
    VECTOR* ApplyMatrixLV(MATRIX* mat, VECTOR* v0, VECTOR* v1);
    SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1);
    //VECTOR* ApplyTransposeMatrixLV(MATRIX* m, VECTOR* v0, VECTOR* v1);
    MATRIX* RotMatrix(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrixXZY(SVECTOR* r, MATRIX* m);
    MATRIX* RotMatrixYXZ(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrixYZX(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrixZXY(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrixZYX(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrix_gte(SVECTOR* r, MATRIX* m);
    //MATRIX* RotMatrixYXZ_gte(SVECTOR* r, MATRIX* m);
    MATRIX* RotMatrixZYX_gte(SVECTOR* r, MATRIX* m);
    MATRIX* RotMatrixX(int32_t r, MATRIX* m);
    MATRIX* RotMatrixY(int32_t r, MATRIX* m);
    MATRIX* RotMatrixZ(int32_t r, MATRIX* m);
    //MATRIX* RotMatrixC(SVECTOR* r, MATRIX* m);
    MATRIX* TransMatrix(MATRIX* m, VECTOR* v);
    MATRIX* ScaleMatrix(MATRIX* m, VECTOR* v);
    //MATRIX* ScaleMatrixL(MATRIX* m, VECTOR* v);
    //MATRIX* TransposeMatrix(MATRIX* m0, MATRIX* m1);
    MATRIX* CompMatrix(MATRIX* m0, MATRIX* m1, MATRIX* m2);
    MATRIX* CompMatrixLV(MATRIX* m0, MATRIX* m1, MATRIX* m2);

    //void MatrixNormal(MATRIX* m, MATRIX* n);
    //void MatrixNormal_0(MATRIX* m, MATRIX* n);
    //void MatrixNormal_1(MATRIX* m, MATRIX* n);
    //void MatrixNormal_2(MATRIX* m, MATRIX* n);

    void SetRotMatrix(MATRIX* m);
    void SetLightMatrix(MATRIX* m);
    void SetColorMatrix(MATRIX* m);
    void SetTransMatrix(MATRIX* m);
    void PushMatrix();
    void PopMatrix();
    void ReadRotMatrix(MATRIX* m);
    void ReadLightMatrix(MATRIX* m);
    void ReadColorMatrix(MATRIX* m);
    void SetRGBcd(CVECTOR* v);
    void SetBackColor(long rbk, long gbk, long bbk);
    void SetFarColor(long rfc, long gfc, long bfc);
    void SetGeomOffset(long ofx, long ofy);
    void SetGeomScreen(long h);
    void ReadSZfifo3(long* sz0, long* sz1, long* sz2);
    void ReadSZfifo4(long* szx, long* sz0, long* sz1, long* sz2);
    void ReadSXSYfifo(long* sxy0, long* sxy1, long* sxy2);
    void ReadRGBfifo(CVECTOR* v0, CVECTOR* v1, CVECTOR* v2);
    void ReadGeomOffset(long* ofx, long* ofy);
    long ReadGeomScreen();

    //void TransRot_32(VECTOR* v0, VECTOR* v1, long* flag);
    //long TransRotPers(SVECTOR* v0, long* sxy, long* p, long* flag);
    //long TransRotPers3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag);

    //void pers_map(int abuf, SVECTOR** vertex, int tex[4][2], uint16_t* dtext);
    //void PhongLine(int istart_x, int iend_x, int p, int q, uint16_t** pixx, int fs, int ft, int i4, int det);

    long RotTransPers(SVECTOR* v0, long* sxy, long* p, long* flag);
    long RotTransPers3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag);
    void RotTrans(SVECTOR* v0, VECTOR* v1, long* flag);
    void RotTransSV(SVECTOR* v0, SVECTOR* v1, long* flag);
    void LocalLight(SVECTOR* v0, VECTOR* v1);
    void LightColor(VECTOR* v0, VECTOR* v1);
    void DpqColorLight(VECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2);
    void DpqColor(CVECTOR* v0, long p, CVECTOR* v1);
    void DpqColor3(CVECTOR* v0, CVECTOR* v1, CVECTOR* v2, long p, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5);
    void Intpl(VECTOR* v0, long p, CVECTOR* v1);
    VECTOR* Square12(VECTOR* v0, VECTOR* v1);
    VECTOR* Square0(VECTOR* v0, VECTOR* v1);
    VECTOR* SquareSL12(SVECTOR* v0, VECTOR* v1);
    VECTOR* SquareSL0(SVECTOR* v0, VECTOR* v1);
    SVECTOR* SquareSS12(SVECTOR* v0, SVECTOR* v1);
    SVECTOR* SquareSS0(SVECTOR* v0, SVECTOR* v1);
    void NormalColor(SVECTOR* v0, CVECTOR* v1);
    void NormalColor3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5);
    void NormalColorDpq(SVECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2);
    void NormalColorDpq3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, long p, CVECTOR* v4, CVECTOR* v5, CVECTOR* v6);
    void NormalColorCol(SVECTOR* v0, CVECTOR* v1, CVECTOR* v2);
    void NormalColorCol3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, CVECTOR* v3, CVECTOR* v4, CVECTOR* v5, CVECTOR* v6);
    void ColorDpq(VECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2);
    void ColorCol(VECTOR* v0, CVECTOR* v1, CVECTOR* v2);
    long NormalClip(long sxy0, long sxy1, long sxy2);
    long AverageZ3(long sz0, long sz1, long sz2);
    long AverageZ4(long sz0, long sz1, long sz2, long sz3);
    void OuterProduct12(VECTOR* v0, VECTOR* v1, VECTOR* v2);
    void OuterProduct0(VECTOR* v0, VECTOR* v1, VECTOR* v2);
    long Lzc(long data);

    long RotTransPers4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* flag);
    void RotTransPersN(SVECTOR* v0, DVECTOR* v1, uint16_t* sz, uint16_t* p, uint16_t* flag, long n);
    //void RotTransPers3N(SVECTOR* v0, DVECTOR* v1, uint16_t* sz, uint16_t* flag, long n);
    //void RotMeshH(short* Yheight, DVECTOR* Vo, uint16_t* sz, uint16_t* flag, short Xoffset, short Zoffset, short m, short n, DVECTOR* base);
    long RotAverage3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag);
    long RotAverage4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* flag);
    long RotNclip3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* otz, long* flag);
    long RotNclip4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* otz, long* flag);
    long RotAverageNclip3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* otz, long* flag);
    long RotAverageNclip4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, long* sxy0, long* sxy1, long* sxy2, long* sxy3, long* p, long* otz, long* flag);
    long RotColorDpq(SVECTOR* v0, SVECTOR* v1, CVECTOR* v2, long* sxy, CVECTOR* v3, long* flag);
    long RotColorDpq3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* flag);
    long RotAverageNclipColorDpq3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* otz, long* flag);
    long RotAverageNclipColorCol3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3, SVECTOR* v4, SVECTOR* v5, CVECTOR* v6, long* sxy0, long* sxy1, long* sxy2, CVECTOR* v7, CVECTOR* v8, CVECTOR* v9, long* otz, long* flag);
    long RotColorMatDpq(SVECTOR* v0, SVECTOR* v1, CVECTOR* v2, long* sxy, CVECTOR* v3, long matc, long flag);
    void ColorMatDpq(SVECTOR* v0, CVECTOR* v1, long p, CVECTOR* v2, long matc);
    void ColorMatCol(SVECTOR* v0, CVECTOR* v1, CVECTOR* v2, long matc);
    void LoadAverage12(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* v2);
    void LoadAverageShort12(SVECTOR* v0, SVECTOR* v1, long p0, long p1, SVECTOR* v2);
    void LoadAverage0(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* v2);
    void LoadAverageShort0(SVECTOR* v0, SVECTOR* v1, long p0, long p1, SVECTOR* v2);
    void LoadAverageByte(uint8_t* v0, uint8_t* v1, long p0, long p1, uint8_t* v2);
    void LoadAverageCol(uint8_t* v0, uint8_t* v1, long p0, long p1, uint8_t* v2);
    //long VectorNormal(VECTOR* v0, VECTOR* v1);
    //long VectorNormalS(VECTOR* v0, SVECTOR* v1);
    //long VectorNormalSS(SVECTOR* v0, SVECTOR* v1);
    long SquareRoot0(long a);
    long SquareRoot12(long a);
    void InvSquareRoot(long a, long* b, long* c);
    //void gteMIMefunc(SVECTOR* otp, SVECTOR* dfp, long n, long p);
    void SetFogFar(long a, long h);
    void SetFogNear(long a, long h);
    void SetFogNearFar(long a, long b, long h);
    //void SubPol4(POL4* p, SPOL* sp, int ndiv);
    //void SubPol3(POL3* p, SPOL* sp, int ndiv);

    int rcos(int a);
    int rsin(int a);
    //int ccos(int a);
    //int csin(int a);
    //int cln(int a);
    //int csqrt(int a);
    //int catan(int a);
    long ratan2(long y, long x);

};