/* $PSLibId: Run-time Library Release 4.7$ */

#pragma once

#include <cstdint>

/*
 *  (C) Copyright 1993-1995 Sony Corporation,Tokyo,Japan. All Rights Reserved
 *
 * 		libgpu.h: Graphic Primitive Structures Database
 *
 * Primitive list:
 *
 *      Name     |Size*1|Shade  |Vertex |Texture| Function
 *      ---------+------+-------+-------+-------+------------------------
 *      POLY_F3  | 5	|Flat   |   3   |OFF    | Flat Triangle
 *      POLY_FT3 | 8	|Flat   |   3   |ON     | Flat Textured Triangle
 *      POLY_G3  | 7	|Gouraud|   3   |OFF    | Gouraud Triangle
 *      POLY_GT3 |10	|Gouraud|   3   |ON     | Gouraud Textured Triangle
 *      POLY_F4  | 6	|Flat   |   4   |OFF    | Flat Quadrangle
 *      POLY_FT4 |10	|Flat   |   4   |ON     | Flat Textured Quadrangle
 *      POLY_G4  | 9	|Gouraud|   4   |OFF    | Gouraud Quadrangle
 *      POLY_GT4 |13	|Gouraud|   4   |ON     | Gouraud Textured Quadrangle
 *      ---------+------+-------+-------+-------+------------------------
 *      LINE_F2  | 4	|Flat   |   2   | -     | unconnected Flat Line 
 *      LINE_G2  | 5	|Gouraud|   2   | -     | unconnected Gouraud Line 
 *      LINE_F3  | 6	|Flat	|   3	| -     | 3-connected Flat Line
 *      LINE_G3  | 8	|Gouraud|   3	| -     | 3-connected Gouraud Line
 *      LINE_F4  | 7	|Flat	|   4	| -    	| 4-connected Flat Line
 *      LINE_G4  |10	|Gouraud|   4	| -    	| 4-connected Gouraud Line
 *      ---------+------+-------+-------+-------+------------------------
 *      SPRT	 | 5	|Flat	|   1   |ON     | free size Sprite
 *      SPRT_16	 | 4	|Flat	|   1   |ON     | 16x16 Sprite
 *      SPRT_8	 | 4	|Flat	|   1   |ON     | 8x8 Sprite
 *      ---------+------+-------+-------+-------+------------------------
 *      TILE	 | 4	|Flat	|   1   |OFF    | free size Sprite
 *      TILE_16	 | 3	|Flat	|   1   |OFF    | 16x16 Sprite
 *      TILE_8	 | 3	|Flat	|   1   |OFF    | 8x8 Sprite
 *      TILE_1	 | 3	|Flat	|   1   |OFF    | 1x1 Sprite
 *      ---------+------+-------+-------+-------+------------------------
 *      DR_TWIN	 | 3	|   -	|   -   | -     | Texture Window
 *      DR_AREA	 | 3	|   -	|   -   | -     | Drawing Area
 *      DR_OFFSET| 3	|   -	|   -   | -     | Drawing Offset
 *      DR_MODE  | 3	|   -	|   -   | -     | Drawing Mode
 *      DR_ENV   |16	|   -	|   -	| -     | Drawing Environment
 *      DR_MOVE  | 6	|   -	|   -	| -     | MoveImage
 *      DR_LOAD  |17	|   -	|   -	| -     | LoadImage
 *      DR_TPAGE | 2    |   -   |   -   | -     | Drawing TPage
 *      DR_STP   | 3    |   -   |   -   | -     | Drawing STP
 *
 *	*1: in long-word
 *
 * Texture Attributes:
 *	abr: ambient rate
 *		abr	0	1	2	3
 *		-------------------------------------
 *		Front	0.5	1.0	0.5	-1.0
 *		Back	0.5	1.0	1.0	 1.0
 *
 *	tp: texture mode
 *		 tp	0	1	2	
 *		 -----------------------------
 *		 depth	4bit	8bit	16bit
 *		 color	CLUT	CLUT	DIRECT
 */

/*
 *	Externals
 */
extern	int (*GPU_printf)(char *fmt, ...);	/* printf() object */

/*
 *	Time-out Cycle
 */
#define WAIT_TIME	0x800000

/*
 *	General Macros
 */
#define limitRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))

/*
 *	Set/Add Vector/Rectangle Attributes
 */
#define setVector(v, _x, _y, _z) \
	(v)->vx = _x, (v)->vy = _y, (v)->vz = _z	

#define applyVector(v, _x, _y, _z, op) \
	(v)->vx op _x, (v)->vy op _y, (v)->vz op _z	

#define copyVector(v0, v1) \
	(v0)->vx = (v1)->vx, (v0)->vy = (v1)->vy, (v0)->vz = (v1)->vz 

#define addVector(v0, v1) \
	(v0)->vx += (v1)->vx,	\
	(v0)->vy += (v1)->vy,	\
	(v0)->vz += (v1)->vz	
	
#define dumpVector(str, v)	\
	GPU_printf("%s=(%d,%d,%d)\n", str, (v)->vx, (v)->vy, (v)->vz)

#define dumpMatrix(x)	\
	GPU_printf("\t%5d,%5d,%5d\n",(x)->m[0][0],(x)->m[0][1],(x)->m[0][2]),\
	GPU_printf("\t%5d,%5d,%5d\n",(x)->m[1][0],(x)->m[1][1],(x)->m[1][2]),\
	GPU_printf("\t%5d,%5d,%5d\n",(x)->m[2][0],(x)->m[2][1],(x)->m[2][2])

#define setRECT(r, _x, _y, _w, _h) \
	(r)->x = (_x),(r)->y = (_y),(r)->w = (_w),(r)->h = (_h)

/*
 *	Set Primitive Attributes
 */
#define setTPage(p,tp,abr,x,y) \
	((p)->tpage = getTPage(tp,abr,x,y))

#define setClut(p,x,y) \
	((p)->clut = getClut(x,y))
					   
/*
 * Set Primitive Colors
 */
#define setRGB0(p,_r0,_g0,_b0)						\
	(p)->r0 = _r0,(p)->g0 = _g0,(p)->b0 = _b0
	
#define setRGB1(p,_r1,_g1,_b1)						\
	(p)->r1 = _r1,(p)->g1 = _g1,(p)->b1 = _b1

#define setRGB2(p,_r2,_g2,_b2)						\
	(p)->r2 = _r2,(p)->g2 = _g2,(p)->b2 = _b2
	
#define setRGB3(p,_r3,_g3,_b3)						\
	(p)->r3 = _r3,(p)->g3 = _g3,(p)->b3 = _b3
	
/*
 * Set Primitive Screen Points
 */
#define setXY0(p,_x0,_y0)						\
	(p)->x0 = (_x0), (p)->y0 = (_y0)				\

#define setXY2(p,_x0,_y0,_x1,_y1)					\
	(p)->x0 = (_x0), (p)->y0 = (_y0),				\
	(p)->x1 = (_x1), (p)->y1 = (_y1)

#define setXY3(p,_x0,_y0,_x1,_y1,_x2,_y2)				\
	(p)->x0 = (_x0), (p)->y0 = (_y0),				\
	(p)->x1 = (_x1), (p)->y1 = (_y1),				\
	(p)->x2 = (_x2), (p)->y2 = (_y2)

#define setXY4(p,_x0,_y0,_x1,_y1,_x2,_y2,_x3,_y3) 			\
	(p)->x0 = (_x0), (p)->y0 = (_y0),				\
	(p)->x1 = (_x1), (p)->y1 = (_y1),				\
	(p)->x2 = (_x2), (p)->y2 = (_y2),				\
	(p)->x3 = (_x3), (p)->y3 = (_y3)

#define setXYWH(p,_x0,_y0,_w,_h)					\
	(p)->x0 = (_x0),      (p)->y0 = (_y0),				\
	(p)->x1 = (_x0)+(_w), (p)->y1 = (_y0),				\
	(p)->x2 = (_x0),      (p)->y2 = (_y0)+(_h),			\
	(p)->x3 = (_x0)+(_w), (p)->y3 = (_y0)+(_h)

/*
 * Set Primitive Width/Height
 */
#define setWH(p,_w,_h)	(p)->w = _w, (p)->h = _h

/*
 * Set Primitive Texture Points
 */
#define setUV0(p,_u0,_v0)						\
	(p)->u0 = (_u0), (p)->v0 = (_v0)				\
	
#define setUV3(p,_u0,_v0,_u1,_v1,_u2,_v2)				\
	(p)->u0 = (_u0), (p)->v0 = (_v0),				\
	(p)->u1 = (_u1), (p)->v1 = (_v1),				\
	(p)->u2 = (_u2), (p)->v2 = (_v2)
	
#define setUV4(p,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3) 			\
	(p)->u0 = (_u0), (p)->v0 = (_v0),				\
	(p)->u1 = (_u1), (p)->v1 = (_v1),				\
	(p)->u2 = (_u2), (p)->v2 = (_v2),				\
	(p)->u3 = (_u3), (p)->v3 = (_v3)

#define setUVWH(p,_u0,_v0,_w,_h)					\
	(p)->u0 = (_u0),      (p)->v0 = (_v0),				\
	(p)->u1 = (_u0)+(_w), (p)->v1 = (_v0),				\
	(p)->u2 = (_u0),      (p)->v2 = (_v0)+(_h),			\
	(p)->u3 = (_u0)+(_w), (p)->v3 = (_v0)+(_h)

	
/*
 * Dump Primivie Parameters
 */
#define dumpRECT(r)	\
	GPU_printf("(%d,%d)-(%d,%d)\n", (r)->x,(r)->y,(r)->w,(r)->h)

#define dumpWH(p)	GPU_printf("(%d,%d)\n", (p)->w,  (p)->h ) 
#define dumpXY0(p)	GPU_printf("(%d,%d)\n", (p)->x0, (p)->y0) 
#define dumpUV0(p)	GPU_printf("(%d,%d)\n", (p)->u0, (p)->v0) 

#define dumpXY2(p)							\
	GPU_printf("(%d,%d)-(%d,%d)\n",					\
	(p)->x0, (p)->y0, (p)->x1, (p)->y1)

#define dumpXY3(p)							\
	GPU_printf("(%d,%d)-(%d,%d)-(%d,%d)\n",				\
	(p)->x0, (p)->y0, (p)->x1, (p)->y1,				\
	(p)->x2, (p)->y2)

#define dumpUV3(p)							\
	GPU_printf("(%d,%d)-(%d,%d)-(%d,%d)\n",				\
	(p)->u0, (p)->v0, (p)->u1, (p)->v1,				\
	(p)->u2, (p)->v2)

#define dumpXY4(p)							\
	GPU_printf("(%d,%d)-(%d,%d)-(%d,%d)-(%d,%d)\n",			\
	(p)->x0, (p)->y0, (p)->x1, (p)->y1,				\
	(p)->x2, (p)->y2, (p)->x3, (p)->y3)

#define dumpUV4(p)							\
	GPU_printf("(%d,%d)-(%d,%d)-(%d,%d)-(%d,%d)\n",			\
	(p)->u0, (p)->v0, (p)->u1, (p)->v1,				\
	(p)->u2, (p)->v2, (p)->u3, (p)->v3)			

#define dumpRGB0(p)							\
	GPU_printf("(%3d,%3d,%3d)\n", (p)->r0, (p)->g0, (p)->b0) 	
		   
#define dumpRGB1(p)							\
	GPU_printf("(%3d,%3d,%3d)\n", (p)->r1, (p)->g1, (p)->b1)	
		   
#define dumpRGB2(p)							\
	GPU_printf("(%3d,%3d,%3d)\n", (p)->r2, (p)->g2, (p)->b2) 
		   
#define dumpRGB3(p)							\
	GPU_printf("(%3d,%3d,%3d)\n", (p)->r3, (p)->g3, (p)->b3) 	

/*
 * Primitive Handling Macros
 */
#define setlen( p, _len) 	(((P_TAG *)(p))->len  = (uint8_t)(_len))
#define setaddr(p, _addr)	(((P_TAG *)(p))->addr = (uint32_t)(_addr))
#define setcode(p, _code)	(((P_TAG *)(p))->code = (uint8_t)(_code))

#define getlen(p)    		(uint8_t)(((P_TAG *)(p))->len)
#define getcode(p)   		(uint8_t)(((P_TAG *)(p))->code)
#define getaddr(p)   		(uint32_t)(((P_TAG *)(p))->addr)

#define nextPrim(p)  		(void *)((((P_TAG *)(p))->addr)|0x80000000)
#define isendprim(p) 		((((P_TAG *)(p))->addr)==0xffffff)

#define addPrim(ot, p)		setaddr(p, getaddr(ot)), setaddr(ot, p)
#define addPrims(ot, p0, p1)	setaddr(p1, getaddr(ot)),setaddr(ot, p0)

#define catPrim(p0, p1)		setaddr(p0, p1)
#define termPrim(p)		setaddr(p, 0xffffffff)

#define setSemiTrans(p, abe) \
	((abe)?setcode(p, getcode(p)|0x02):setcode(p, getcode(p)&~0x02))

#define setShadeTex(p, tge) \
	((tge)?setcode(p, getcode(p)|0x01):setcode(p, getcode(p)&~0x01))

#define getTPage(tp, abr, x, y) 					\
	 ((((tp)&0x3)<<7)|(((abr)&0x3)<<5)|(((y)&0x100)>>4)|(((x)&0x3ff)>>6)| \
	 (((y)&0x200)<<2))

#define getClut(x, y) \
	(((y)<<6)|(((x)>>4)&0x3f))

#define dumpTPage(tpage)						\
	GPU_printf("tpage: (%d,%d,%d,%d)\n",				\
			   ((tpage)>>7)&0x003,((tpage)>>5)&0x003,	\
			   ((tpage)<<6)&0x7c0,				\
			   (((tpage)<<4)&0x100)+(((tpage)>>2)&0x200))

#define dumpClut(clut) \
	GPU_printf("clut: (%d,%d)\n", (clut&0x3f)<<4, (clut>>6))

#define _get_mode(dfe, dtd, tpage)	\
		((0xe1000000)|((dtd)?0x0200:0)| \
		((dfe)?0x0400:0)|((tpage)&0x9ff))

#define setDrawTPage(p, dfe, dtd, tpage)	\
	setlen(p, 1),	\
	((uint32_t *)(p))[1] = _get_mode(dfe, dtd, tpage)

#define _get_tw(tw)	\
		(tw ? ((0xe2000000)|((((tw)->y&0xff)>>3)<<15)| \
		((((tw)->x&0xff)>>3)<<10)|(((~((tw)->h-1)&0xff)>>3)<<5)| \
		(((~((tw)->w-1)&0xff)>>3))) : 0)

#define setTexWindow(p, tw)			\
	setlen(p, 2),				\
	((uint32_t *)(p))[1] = _get_tw(tw),	\
	((uint32_t *)(p))[2] = 0

#define _get_len(rect)	\
		(((rect)->w*(rect)->h+1)/2+4)

#define setDrawLoad(pt, rect)					\
	(_get_len(rect) <= 16) ? (				\
		(setlen(pt, _get_len(rect))),			\
		((pt)->code[0] = 0xa0000000),			\
		((pt)->code[1] = *((uint32_t *)&(rect)->x)),	\
		((pt)->code[2] = *((uint32_t *)&(rect)->w)),	\
		((pt)->p[_get_len(rect)-4] = 0x01000000)	\
	) : ( \
		(setlen(pt,0)) \
	)

#define setDrawStp(p, pbw) 				\
        setlen(p, 2),					\
        ((uint32_t *)p)[1] = 0xe6000000|(pbw?0x01:0),	\
        ((uint32_t *)p)[2] = 0

#define setDrawMode(p, dfe, dtd, tpage, tw) 		\
        setlen(p, 2),					\
        ((uint32_t *)p)[1] = _get_mode(dfe, dtd, tpage),	\
        ((uint32_t *)p)[2] = _get_tw((PSX_RECT *)tw)

	
/*	Primitive 	Lentgh		Code				*/
/*--------------------------------------------------------------------	*/
/*									*/
#define setPolyF3(p)	setlen(p, 4),  setcode(p, 0x20)
#define setPolyFT3(p)	setlen(p, 7),  setcode(p, 0x24)
#define setPolyG3(p)	setlen(p, 6),  setcode(p, 0x30)
#define setPolyGT3(p)	setlen(p, 9),  setcode(p, 0x34)
#define setPolyF4(p)	setlen(p, 5),  setcode(p, 0x28)
#define setPolyFT4(p)	setlen(p, 9),  setcode(p, 0x2c)
#define setPolyG4(p)	setlen(p, 8),  setcode(p, 0x38)
#define setPolyGT4(p)	setlen(p, 12), setcode(p, 0x3c)

#define setSprt8(p)	setlen(p, 3),  setcode(p, 0x74)
#define setSprt16(p)	setlen(p, 3),  setcode(p, 0x7c)
#define setSprt(p)	setlen(p, 4),  setcode(p, 0x64)

#define setTile1(p)	setlen(p, 2),  setcode(p, 0x68)
#define setTile8(p)	setlen(p, 2),  setcode(p, 0x70)
#define setTile16(p)	setlen(p, 2),  setcode(p, 0x78)
#define setTile(p)	setlen(p, 3),  setcode(p, 0x60)
#define setLineF2(p)	setlen(p, 3),  setcode(p, 0x40)
#define setLineG2(p)	setlen(p, 4),  setcode(p, 0x50)
#define setLineF3(p)	setlen(p, 5),  setcode(p, 0x48),(p)->pad = 0x55555555
#define setLineG3(p)	setlen(p, 7),  setcode(p, 0x58),(p)->pad = 0x55555555, \
			(p)->p2 = 0
#define setLineF4(p)	setlen(p, 6),  setcode(p, 0x4c),(p)->pad = 0x55555555
#define setLineG4(p)	setlen(p, 9),  setcode(p, 0x5c),(p)->pad = 0x55555555, \
			(p)->p2 = 0, (p)->p3 = 0

#pragma pack(push, 1)
	
/*
 * Rectangle:
 */
typedef struct {
	short x, y;		/* offset point on VRAM */
	short w, h;		/* width and height */
} PSX_RECT;

typedef struct {
	int x, y;		/* offset point on VRAM */
	int w, h;		/* width and height */
} PSX_RECT32;

/*
 * Environment 
 */
typedef struct {
	uint32_t	tag;
	uint32_t	code[15];
} DR_ENV;				/* Packed Drawing Environment */
	       
typedef struct {
	PSX_RECT	clip;		/* clip area */
	short	ofs[2];		/* drawing offset */
	PSX_RECT	tw;		/* texture window */
	uint16_t tpage;		/* texture page */	
	uint8_t	dtd;		/* dither flag (0:off, 1:on) */
	uint8_t	dfe;		/* flag to draw on display area (0:off 1:on) */
	uint8_t	isbg;		/* enable to auto-clear */
	uint8_t	r0, g0, b0;	/* initital background color */
	DR_ENV	dr_env;		/* reserved */
} DRAWENV;
	       
typedef struct {
	PSX_RECT	disp;		/* display area */
	PSX_RECT	screen;		/* display start point */
	uint8_t	isinter;	/* interlace 0: off 1: on */
	uint8_t	isrgb24;	/* RGB24 bit mode */
	uint8_t	pad0, pad1;	/* reserved */
} DISPENV;

/*
 * Polygon Primitive Definitions
 */
typedef struct {
	unsigned	addr: 24;
	unsigned 	len:   8;
	uint8_t		r0, g0, b0, code;
} P_TAG;
	
typedef struct {
	uint8_t	r0, g0, b0, code;
} P_CODE;
	
typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
} POLY_F3;				/* Flat Triangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	short	x3,	y3;
} POLY_F4;				/* Flat Quadrangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
	short	x1,	y1;
	uint8_t	u1, v1;	uint16_t	tpage;
	short	x2,	y2;
	uint8_t	u2, v2;	uint16_t	pad1;
} POLY_FT3;				/* Flat Textured Triangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
	short	x1,	y1;
	uint8_t	u1, v1;	uint16_t	tpage;
	short	x2,	y2;
	uint8_t	u2, v2;	uint16_t	pad1;
	short	x3,	y3;
	uint8_t	u3, v3;	uint16_t	pad2;
} POLY_FT4;				/* Flat Textured Quadrangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	r1, g1, b1, pad1;
	short	x1,	y1;
	uint8_t	r2, g2, b2, pad2;
	short	x2,	y2;
} POLY_G3;				/* Gouraud Triangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	r1, g1, b1, pad1;
	short	x1,	y1;
	uint8_t	r2, g2, b2, pad2;
	short	x2,	y2;
	uint8_t	r3, g3, b3, pad3;
	short	x3,	y3;
} POLY_G4;				/* Gouraud Quadrangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
	uint8_t	r1, g1, b1, p1;
	short	x1,	y1;
	uint8_t	u1, v1;	uint16_t	tpage;
	uint8_t	r2, g2, b2, p2;
	short	x2,	y2;
	uint8_t	u2, v2;	uint16_t	pad2;
} POLY_GT3;				/* Gouraud Textured Triangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
	uint8_t	r1, g1, b1, p1;
	short	x1,	y1;
	uint8_t	u1, v1;	uint16_t	tpage;
	uint8_t	r2, g2, b2, p2;
	short	x2,	y2;
	uint8_t	u2, v2;	uint16_t	pad2;
	uint8_t	r3, g3, b3, p3;
	short	x3,	y3;
	uint8_t	u3, v3;	uint16_t	pad3;
} POLY_GT4;				/* Gouraud Textured Quadrangle */

/*
 * Line Primitive Definitions
 */
typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
} LINE_F2;				/* Unconnected Flat Line */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	r1, g1, b1, p1;
	short	x1,	y1;
} LINE_G2;				/* Unconnected Gouraud Line */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	uint32_t	pad;
} LINE_F3;				/* 2 connected Flat Line */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	r1, g1, b1, p1;
	short	x1,	y1;
	uint8_t	r2, g2, b2, p2;
	short	x2,	y2;
	uint32_t	pad;
} LINE_G3;				/* 2 connected Gouraud Line */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	short	x3,	y3;
	uint32_t	pad;
} LINE_F4;				/* 3 connected Flat Line Quadrangle */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	r1, g1, b1, p1;
	short	x1,	y1;
	uint8_t	r2, g2, b2, p2;
	short	x2,	y2;
	uint8_t	r3, g3, b3, p3;
	short	x3,	y3;
	uint32_t	pad;
} LINE_G4;				/* 3 connected Gouraud Line */

/*
 * Sprite Primitive Definitions
 */
typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
	short	w,	h;
} SPRT;					/* free size Sprite */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
} SPRT_16;				/* 16x16 Sprite */
	       
typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	uint8_t	u0, v0;	uint16_t	clut;
} SPRT_8;				/* 8x8 Sprite */
	       
/*
 * Tile Primitive Definitions
 */
typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
	short	w,	h;
} TILE;					/* free size Tile */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_16;				/* 16x16 Tile */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_8;				/* 8x8 Tile */

typedef struct {
	uint32_t	tag;
	uint8_t	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_1;				/* 1x1 Tile */

/*
 *  Special Primitive Definitions
 */
typedef struct {
	uint32_t	tag;
	uint32_t	code[2];
} DR_MODE;				/* Drawing Mode */

typedef struct {
	uint32_t	tag;
	uint32_t	code[2];
} DR_TWIN;				/* Texture Window */
	       
typedef struct {
	uint32_t	tag;
	uint32_t	code[2];
} DR_AREA;				/* Drawing Area */
	       
typedef struct {
	uint32_t	tag;
	uint32_t	code[2];
} DR_OFFSET;				/* Drawing Offset */
	       
typedef struct {			/* MoveImage */
	uint32_t	tag;
	uint32_t	code[5];
} DR_MOVE;

typedef struct {			/* LoadImage */
	uint32_t	tag;
	uint32_t	code[3];
	uint32_t	p[13];
} DR_LOAD;

typedef	struct {
	uint32_t	tag;
	uint32_t	code[1];
} DR_TPAGE;				/* Drawing TPage */

typedef struct {
	uint32_t  tag;
	uint32_t  code[2];
} DR_STP;                               /* Drawing STP */

#pragma pack(pop)

/*
 *	Font Stream Parameters
 */
#define FNT_MAX_ID	8	/* max number of stream ID */
#define FNT_MAX_SPRT	1024	/* max number of sprites in all streams */
