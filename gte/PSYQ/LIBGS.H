/*
 * $PSLibId: Run-time Library Release 4.7$
 */

/*
 * libgs.h: Graphic Library Header 
 *
 *
 * Version 1.**	Apr,  8, 1994 
 *
 * Copyright (C) 1993 by Sony Corporation All rights Reserved 
 */

#pragma once

#include <cstdint>

/*** packet peripheral pointer ***/
typedef unsigned char PACKET;

#define PSBANK 0x80000000
/*** --- Zsort resolution --- ***/
#define ZRESOLUTION     0x3fff

/*** --- coordinate keyword - ***/
#define WORLD NULL
#define SCREEN ((GsCOORDINATE2 *)0x0001)

#pragma pack(push, 1)


typedef struct {
	VECTOR  scale;
	SVECTOR rotate;
	VECTOR  trans;
}       GsCOORD2PARAM;

typedef struct _GsCOORDINATE2 {
	unsigned long flg;
	MATRIX  coord;
	MATRIX  workm;
	GsCOORD2PARAM *param;
	struct _GsCOORDINATE2 *super;
	struct _GsCOORDINATE2 *sub;
}       GsCOORDINATE2;

typedef struct {
	MATRIX  view;
	GsCOORDINATE2 *super;
}       GsVIEW2;

typedef struct {
	long    vpx, vpy, vpz;
	long    vrx, vry, vrz;
	long    rz;
	GsCOORDINATE2 *super;
}       GsRVIEW2;

typedef struct {
	int     vx, vy, vz;
	unsigned char r, g, b;
}       GsF_LIGHT;


typedef struct {
	unsigned p:24;
	unsigned char num:8;
}       GsOT_TAG;


typedef struct {
	unsigned long length;
	GsOT_TAG *org;
	unsigned long offset;
	unsigned long point;
	GsOT_TAG *tag;
}       GsOT;

typedef struct {
	unsigned long attribute;/* pers,trans,rotate,disp */
	GsCOORDINATE2 *coord2;	/* local dmatrix */
	unsigned long *tmd;
	unsigned long id;
}       GsDOBJ2;

typedef struct {
	unsigned long attribute;/* pers,trans,rotate,disp */
	GsCOORDINATE2 *coord2;	/* local dmatrix */
	unsigned long *pmd;	/* pmd top address */
	unsigned long *base;	/* object base address */
	unsigned long *sv;	/* shared vertex base */
	unsigned long id;
}       GsDOBJ3;

typedef struct {
	unsigned long attribute;/* pers,trans,rotate,disp */
	GsCOORDINATE2 *coord2;	/* local dmatrix */
	unsigned long *tmd;
	unsigned long id;
}       GsDOBJ4;

typedef struct {
	unsigned long attribute;
	GsCOORDINATE2 *coord2;
	unsigned long *tmd;
	unsigned long *packet;
	unsigned long id;
}       GsDOBJ5;

typedef struct {
	unsigned long attribute;
	short   x, y;
	unsigned short w, h;
	unsigned short tpage;
	unsigned char u, v;
	short   cx, cy;
	unsigned char r, g, b;
	short   mx, my;
	short   scalex, scaley;
	long    rotate;
}       GsSPRITE;

typedef struct {
	unsigned char u, v;
	unsigned short cba;
	unsigned short flag;
	unsigned short tpage;
}       GsCELL;

typedef struct {
	unsigned char cellw, cellh;
	unsigned short ncellw, ncellh;
	GsCELL *base;
	unsigned short *index;
}       GsMAP;

typedef struct {
	unsigned long attribute;
	short   x, y;
	short   w, h;
	short   scrollx, scrolly;
	unsigned char r, g, b;
	GsMAP  *map;
	short   mx, my;
	short   scalex, scaley;
	long    rotate;
}       GsBG;

typedef struct {
	unsigned long attribute;
	short   x0, y0;
	short   x1, y1;
	unsigned char r, g, b;
}       GsLINE;

typedef struct {
	unsigned long attribute;
	short   x0, y0;
	short   x1, y1;
	unsigned char r0, g0, b0;
	unsigned char r1, g1, b1;
}       GsGLINE;

typedef struct {
	unsigned long attribute;
	short   x, y;
	unsigned short w, h;
	unsigned char r, g, b;
}       GsBOXF;

typedef struct {
	short   dqa;
	long    dqb;
	unsigned char rfc, gfc, bfc;
}       GsFOGPARAM;


typedef struct {
	unsigned long pmode;
	short   px, py;
	unsigned short pw, ph;
	unsigned long *pixel;
	short   cx, cy;
	unsigned short cw, ch;
	unsigned long *clut;
}       GsIMAGE;

typedef struct {
	short   offx, offy;
}       _GsPOSITION;

typedef struct {
	GsDOBJ2 *top;
	int     nobj;
	int     maxobj;
}       GsOBJTABLE2;

typedef struct {
	PACKET
	* (*f3[2][3]) ();
	PACKET
	* (*nf3[2]) ();
	PACKET
	* (*g3[2][3]) ();
	PACKET
	* (*ng3[2]) ();
	PACKET
	* (*tf3[2][3]) ();
	PACKET
	* (*ntf3[2]) ();
	PACKET
	* (*tg3[2][3]) ();
	PACKET
	* (*ntg3[2]) ();
	PACKET
	* (*f4[2][3]) ();
	PACKET
	* (*nf4[2]) ();
	PACKET
	* (*g4[2][3]) ();
	PACKET
	* (*ng4[2]) ();
	PACKET
	* (*tf4[2][3]) ();
	PACKET
	* (*ntf4[2]) ();
	PACKET
	* (*tg4[2][3]) ();
	PACKET
	* (*ntg4[2]) ();
	PACKET
	* (*f3g[3])();
	PACKET
	* (*g3g[3])();
	PACKET
	* (*f4g[3])();
	PACKET
	* (*g4g[3])();
}       _GsFCALL;


#define GsDivMODE_NDIV 0
#define GsDivMODE_DIV  1
#define GsLMODE_NORMAL 0
#define GsLMODE_FOG    1
#define GsLMODE_LOFF   2

/*
 * libgs macro 
 */
#define GsOFSGTE 0
#define GsOFSGPU 4
#define GsINTER  1
#define GsNONINTER 0
#define GsRESET0 0
#define GsRESET3 (3<<4)

/*
 * object attribute set macro 
 */
#define GsLDIM0 0
#define GsLDIM1 1
#define GsLDIM2 2
#define GsLDIM3 3
#define GsLDIM4 4
#define GsLDIM5 5
#define GsLDIM6 6
#define GsLDIM7 7
#define GsFOG   (1<<3)
#define GsMATE  (1<<4)
#define GsLLMOD (1<<5)
#define GsLOFF  (1<<6)
#define GsZIGNR (1<<7)
#define GsNBACKC (1<<8)
#define GsDIV1   (1<<9)
#define GsDIV2   (2<<9)
#define GsDIV3   (3<<9)
#define GsDIV4	 (4<<9)
#define GsDIV5	 (5<<9)
#define GsAZERO  (0<<28)
#define GsAONE   (1<<28)
#define GsATWO   (2<<28)
#define GsATHREE (3<<28)
#define GsALON   (1<<30)
#define GsDOFF   (1<<31)
/*
 * BG/sprite attribute set macro 
 */
#define GsPERS   (1<<26)
#define GsROTOFF (1<<27)

#define GsIncFrame()  (PSDCNT++, PSDCNT= PSDCNT?PSDCNT:1, \
                      (PSDIDX= (PSDIDX==0?1:0)))

#define GsUpdateCoord()  (PSDCNT++, PSDCNT= PSDCNT?PSDCNT:1)

#define GsSetAzwh(z,w,h)    GsADIVZ = (z),GsADIVW = (w),GsADIVH = (h);

#define GsTMDFlagGRD	0x04

/*
 * FLIP macro for GsSort[Fast]SpriteB
 */
#define GsHFLIP		0x01
#define GsVFLIP		0x02

/*
 * TMD structure 
 */
/*** GTE PACKET to-GPU command '<packet-name>.code' ***/
#define GPU_COM_F3    0x20
#define GPU_COM_TF3   0x24
#define GPU_COM_G3    0x30
#define GPU_COM_TG3   0x34

#define GPU_COM_F4    0x28
#define GPU_COM_TF4   0x2c
#define GPU_COM_G4    0x38
#define GPU_COM_TG4   0x3c

#define GPU_COM_NF3   0x21
#define GPU_COM_NTF3  0x25
#define GPU_COM_NG3   0x31
#define GPU_COM_NTG3  0x35

#define GPU_COM_NF4   0x29
#define GPU_COM_NTF4  0x2d
#define GPU_COM_NG4   0x39
#define GPU_COM_NTG4  0x3d


/*** TMD structure ****/
typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
}       TMD_P_F3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
}       TMD_P_G3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t	r0, g0, b0, code;
	std::uint8_t	r1, g1, b1, dummy1;
	std::uint8_t	r2, g2, b2, dummy2;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
}       TMD_P_F3G;

typedef struct {
	std::uint32_t tag;
	std::uint8_t	r0, g0, b0, code;
	std::uint8_t	r1, g1, b1, dummy1;
	std::uint8_t	r2, g2, b2, dummy2;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
}       TMD_P_G3G;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t v0, v1;
	std::uint16_t v2, p;
}       TMD_P_NF3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint8_t  r1, g1, b1, p1;
	std::uint8_t  r2, g2, b2, p2;
	std::uint16_t v0, v1;
	std::uint16_t v2, p;
}       TMD_P_NG3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
	std::uint16_t v3, p;
}       TMD_P_F4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
	std::uint16_t n3, v3;
}       TMD_P_G4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t	r0, g0, b0, code;
	std::uint8_t	r1, g1, b1, dummy1;
	std::uint8_t	r2, g2, b2, dummy2;
	std::uint8_t	r3, g3, b3, dummy3;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
	std::uint16_t v3, dummy4;
}       TMD_P_F4G;

typedef struct {
	std::uint32_t tag;
	std::uint8_t	r0, g0, b0, code;
	std::uint8_t	r1, g1, b1, dummy1;
	std::uint8_t	r2, g2, b2, dummy2;
	std::uint8_t	r3, g3, b3, dummy3;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
	std::uint16_t n3, v3;
}       TMD_P_G4G;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint16_t v0, v1;
	std::uint16_t v2, v3;
}       TMD_P_NF4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  r0, g0, b0, code;
	std::uint8_t  r1, g1, b1, p1;
	std::uint8_t  r2, g2, b2, p2;
	std::uint8_t  r3, g3, b3, p3;
	std::uint16_t v0, v1;
	std::uint16_t v2, v3;
}       TMD_P_NG4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
}       TMD_P_TF3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
}       TMD_P_TG3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  r0, g0, b0, p1;
	std::uint16_t v0, v1;
	std::uint16_t v2, p2;
}       TMD_P_TNF3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  r0, g0, b0, p1;
	std::uint8_t  r1, g1, b1, p2;
	std::uint8_t  r2, g2, b2, p3;
	std::uint16_t v0, v1;
	std::uint16_t v2, p4;
}       TMD_P_TNG3;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  tu3, tv3;
	std::uint16_t p1;
	std::uint16_t n0, v0;
	std::uint16_t v1, v2;
	std::uint16_t v3, p2;
}       TMD_P_TF4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  tu3, tv3;
	std::uint16_t p1;
	std::uint16_t n0, v0;
	std::uint16_t n1, v1;
	std::uint16_t n2, v2;
	std::uint16_t n3, v3;
}       TMD_P_TG4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  tu3, tv3;
	std::uint16_t p1;
	std::uint8_t  r0, g0, b0, p2;
	std::uint16_t v0, v1;
	std::uint16_t v2, v3;
}       TMD_P_TNF4;

typedef struct {
	std::uint32_t tag;
	std::uint8_t  tu0, tv0;
	std::uint16_t clut;
	std::uint8_t  tu1, tv1;
	std::uint16_t tpage;
	std::uint8_t  tu2, tv2;
	std::uint16_t p0;
	std::uint8_t  tu3, tv3;
	std::uint16_t p1;
	std::uint8_t  r0, g0, b0, p2;
	std::uint8_t  r1, g1, b1, p3;
	std::uint8_t  r2, g2, b2, p4;
	std::uint8_t  r3, g3, b3, p5;
	std::uint16_t v0, v1;
	std::uint16_t v2, v3;
}       TMD_P_TNG4;

struct TMD_STRUCT {
	std::uint32_t *vertop;         /* vertex top address of TMD format */
	std::uint32_t  vern;           /* the number of vertex of TMD format */
	std::uint32_t *nortop;         /* normal top address of TMD format */
	std::uint32_t  norn;           /* the number of normal of TMD format */
	std::uint32_t *primtop;        /* primitive top address of TMD format */
	std::uint32_t  primn;          /* the number of primitives of TMD format */
	std::uint32_t  scale;          /* the scale factor of TMD format */
};

/*
 * active sub divide structure 
 *
 */

#define minmax4(x1,x2,x3,x4,x5,x6) x1>x2?(x6=x1,x5=x2):(x5=x1,x6=x2),\
                                   x3>x6?x6=x3:x3<x5?x5=x3:0,\
                                   x4>x6?x6=x4:x4<x5?x5=x4:0

#define minmax3(x1,x2,x3,x4,x5)    x1>x2?(x5=x1,x4=x2):(x4=x1,x5=x2),\
                                   x3>x5?x5=x3:x3<x4?x4=x3:0


typedef struct {
	short   vx, vy, vz;
	std::uint8_t  tu, tv;
}       VERT;

typedef struct {
	short   vx, vy, vz;
	std::uint8_t  tu, tv;
	CVECTOR col;
}       VERTC;


typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg0;		/* gte flag */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_FT4 si;		/* work packet */
}       GsADIV_FT4;

typedef struct {
	VERT    vt[4];
}       GsADIV_P_FT4;



typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg0;		/* gte flag */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_GT4 si;		/* work packet */
}       GsADIV_GT4;

typedef struct {
	VERTC   vt[4];
}       GsADIV_P_GT4;


typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg0;		/* gte flag */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_G4 si;		/* work packet */
}       GsADIV_G4;

typedef struct {
	VERTC   vt[4];
}       GsADIV_P_G4;

typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg0;		/* gte flag */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_F4 si;		/* work packet */
}       GsADIV_F4;

typedef struct {
	VERT    vt[4];
}       GsADIV_P_F4;


typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_FT3 si;		/* work packet */
}       GsADIV_FT3;

typedef struct {
	VERT    vt[3];
}       GsADIV_P_FT3;

typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_GT3 si;		/* work packet */
}       GsADIV_GT3;

typedef struct {
	VERTC   vt[3];
}       GsADIV_P_GT3;

typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_G3 si;		/* work packet */
}       GsADIV_G3;

typedef struct {
	VERTC   vt[3];
}       GsADIV_P_G3;

typedef struct {
	std::uint32_t  limit;		/* divide limit */
	long    hwd, vwd;	/* dummy */
	int     shift;		/* OT shift */
	std::uint32_t *org;		/* OT org */
	std::uint32_t *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    adivz;		/* active divide codition z */
	short   adivw, adivh;	/* active divide condition w,h */
	long    flg;		/* gte flag */
	short   minx, miny, maxx, maxy;	/* polygon min-max */
	short   hwd0, vwd0;	/* resolution of screen */
	std::uint32_t *tag;		/* work temprly for addPrim */
	POLY_F3 si;		/* work packet */
}       GsADIV_F3;

typedef struct {
	VERT    vt[3];
}       GsADIV_P_F3;

#pragma pack(pop)



/* EXTERN */
extern PSX_RECT CLIP2;		/* clipping area */
extern short PSDBASEX[2], PSDBASEY[2];	/* double buffer base */
extern short PSDIDX;		/* double buffer index */
extern std::uint32_t PSDCNT;		/* frame counter for using matrix cache */
extern _GsPOSITION POSITION;	/* 2d offset */
extern DRAWENV GsDRAWENV;	/* DRAWENV of Gs */
extern DISPENV GsDISPENV;	/* DISPENV of Gs */
extern MATRIX GsWSMATRIX;	/* Current World-Screen Matrix of Gs */
extern MATRIX GsWSMATRIX_ORG;	/* Original World-Screen Matrix of Gs */
extern long HWD0, VWD0;		/* rezolution of Holyzontal and Vertical */
extern MATRIX GsLIGHTWSMATRIX;	/* World-Screen Light Matrix of Gs */
extern MATRIX GsIDMATRIX;	/* Unit Matrix */
extern MATRIX GsIDMATRIX2;	/* Unit Matrix including Aspect retio */
extern PACKET *GsOUT_PACKET_P;	/* Work Base pointer */
extern long GsADIVZ;		/* Active sub divide condition (z) */
extern short GsADIVW, GsADIVH;	/* Active sub divide condition (w,h) */
extern int GsLIGHT_MODE;	/* lighting mode global */
extern std::uint32_t GsMATE_C, GsLMODE, GsLIGNR, GsLIOFF, GsZOVER, GsBACKC, GsNDIV;
extern std::uint32_t GsTRATE, GsTON, GsDISPON;
