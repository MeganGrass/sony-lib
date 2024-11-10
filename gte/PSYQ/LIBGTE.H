/*
 * $PSLibId: Run-time Library Release 4.7$ 
 */


/*
 *  (C) Copyright 1993/1994/1995 Sony Computer Entertainment ,Tokyo,Japan.
 *				All Rights Reserved
 *
 * 		libgte.h: Geometry Basic Structures Database
 *
 *$Id: libgte.h,v 1.35 1998/03/10 03:52:35 noda Exp $
 */

#pragma once

#include <cstdint>



/*
 * Geometry Structures:
 */
#define	ONE		4096	/* GTE regards 4096 as 1.0 */
#define TMPVTXNUM	16	/* Clip Function Header */
#define OUTVTXNUM	10

#pragma pack(push, 1)

typedef struct  {
	std::int16_t m[3][3];	/* 3x3 rotation matrix */
	std::uint16_t pad;
	std::int32_t t[3];		/* transfer vector */
} MATRIX;

typedef struct {		/* long word type 3D vector */
	long	vx, vy;
	long	vz, pad;
} VECTOR;

typedef struct {		/* short word type 3D vector */	
	short	vx, vy;
	short	vz, pad;
} SVECTOR;

typedef struct {		/* color type vector */	
	uint8_t	r, g, b, cd;
} CVECTOR;

typedef struct {		/* 2D short vector */
	short vx, vy;
} DVECTOR;


typedef struct {		
	SVECTOR v;		/* Object(Local) 3D Vertex 	*/
	VECTOR sxyz;		/* Screen 3D Vertex		*/
	DVECTOR sxy;		/* Screen 2D Vertex		*/
	CVECTOR rgb;		/* Vertex Color Data	 	*/
	short txuv,pad;		/* Texture Mapping Data 	*/
	long chx,chy;		/* Clip Window Data 		*/
} EVECTOR;

typedef struct {
	SVECTOR v;
	uint8_t uv[2]; uint16_t pad;	/*  */
	CVECTOR c;
	DVECTOR sxy;		
	uint32_t  sz;		/* clip z-data */		
} RVECTOR;			/* division vertex data vector */


typedef struct {
	RVECTOR r01,r12,r20;
	RVECTOR	*r0,*r1,*r2;
	uint32_t*rtn;
} CRVECTOR3;			/* recursive vector for triangles */

typedef struct {
	uint32_t 	ndiv;		/* number of divisions */
	uint32_t 	pih,piv;	/* clipping area */
	uint16_t clut,tpage;
	CVECTOR	rgbc;
	uint32_t*ot;
	RVECTOR r0,r1,r2;
	CRVECTOR3 cr[5];	
} DIVPOLYGON3;			/* division buffer for triangles */

typedef struct {
	RVECTOR r01,r02,r31,r32,rc;
	RVECTOR	*r0,*r1,*r2,*r3;
	uint32_t*rtn;
} CRVECTOR4;			/* recursive vector for four-sided polygons */

typedef struct {
	uint32_t 	ndiv;		/* number of divisions */
	uint32_t 	pih,piv;	/* clipping area */
	uint16_t clut,tpage;
	CVECTOR	rgbc;
	uint32_t*ot;
	RVECTOR r0,r1,r2,r3;
	CRVECTOR4 cr[5];	
} DIVPOLYGON4;			/* division buffer for four-sided polygons */

typedef struct {
        short   xy[3];
        short   uv[2];
        short   rgb[3];
} SPOL;

                                        /*polygon: 41 bytes/1 polygon*/
typedef struct {
        short   sxy[4][2];              /*0..7*/
        short   sz[4][2];               /*8..15sz[][1] is dummy*/
        short   uv[4][2];               /*16..23*/
        short   rgb[4][3];              /*23..34*/
        short   code;                   /*35...  F4:5,TF4:6,G4:7,TG4:8*/
} POL4;

typedef struct {
        short   sxy[3][2];
        short   sz[3][2];               /*sz[][1] is dummy*/
        short   uv[3][2];
        short   rgb[3][3];
        short   code;                   /*F3:1,TF3:2,G3:3,TG3:4*/
} POL3;

typedef struct {
        SVECTOR         *v;                     /*shared vertices*/
        SVECTOR         *n;                     /*shared normals*/
        SVECTOR         *u;                     /*shared texture addresses*/
        CVECTOR         *c;                     /*shared colors*/
		uint32_t          len;                    /*mesh length(=#vertex)*/
} TMESH;

typedef struct {
        SVECTOR         *v;                     /*shared vertices*/
        SVECTOR         *n;                     /*shared normals*/
        SVECTOR         *u;                     /*shared texture addresses*/
        CVECTOR         *c;                     /*shared colors*/
		uint32_t          lenv;                   /*mesh length_V(=#vertex_V)*/
		uint32_t          lenh;                   /*mesh length_H(=#vertex_H)*/
} QMESH;

#pragma pack(pop)