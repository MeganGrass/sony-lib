/* $PSLibId: Run-time Library Release 4.7$ */

/*
*
*	https://github.com/OpenDriver2/PsyCross
*
*/

/*
 * Type 1 functions
 */

 // ctc2 21-23
#define gte_ldfc( r0 ) \
	{	CTC2(*(uint32_t*)((char*)(r0)+0), 21);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 22);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 23);}

// mtc2 0-1
#define gte_ldv0( r0 )\
	{	MTC2(*(uint32_t*)((char*)(r0)+0), 0);\
		MTC2(*(uint32_t*)((char*)(r0)+4), 1);}

// mtc2 2-3
#define gte_ldv1( r0 )\
	{	MTC2(*(uint32_t*)((char*)(r0)+0), 2);\
		MTC2(*(uint32_t*)((char*)(r0)+4), 3);}

// mtc2 4-5
#define gte_ldv2( r0 )\
	{	MTC2(*(uint32_t*)((char*)(r0)+0), 4);\
		MTC2(*(uint32_t*)((char*)(r0)+4), 5);}

// mtc2 0-5
#define gte_ldv3( r0, r1, r2 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)+0), 0);\
		MTC2(*(uint32_t*)((char*)(r0)+4), 1);\
		MTC2(*(uint32_t*)((char*)(r1)+0), 2);\
		MTC2(*(uint32_t*)((char*)(r1)+4), 3);\
		MTC2(*(uint32_t*)((char*)(r2)+0), 4);\
		MTC2(*(uint32_t*)((char*)(r2)+4), 5);}

// load mtc2 9,10,11
#define gte_ldclmv( r0 ) \
	{	MTC2(*(uint16_t*)((char*)(r0)), 9); \
		MTC2(*(uint16_t*)((char*)(r0)+6), 10); \
		MTC2(*(uint16_t*)((char*)(r0)+12), 11);	}

// lwc2 9-11
#define gte_ldlvl( r0 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 9); \
		MTC2(*(uint32_t*)((char*)(r0)+4), 10);\
		MTC2(*(uint32_t*)((char*)(r0)+8), 11);}

// mtc2 9,10,11
#define gte_ldsv( r0 ) \
	{	MTC2(*(uint16_t*)((char*)(r0)), 9);\
		MTC2(*(uint16_t*)((char*)(r0)+2), 10);\
		MTC2(*(uint16_t*)((char*)(r0)+4), 11); }

// mtc2 9,10
#define gte_ldbv( r0 ) \
	{	MTC2(*((char*)(r0)), 9);\
		MTC2(*((char*)(r0)+1), 10); }

// mtc2 9,10,11
#define gte_ldcv( r0 ) \
	{	MTC2(*((char*)(r0)), 9);\
		MTC2(*((char*)(r0)+1), 10);\
		MTC2(*((char*)(r0)+2), 11); }

// lwc2 6
#define gte_ldrgb( r0 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 6);	}

// lwc2 6,20,21,22
#define gte_ldrgb3( r0, r1, r2 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 20); \
		MTC2(*(uint32_t*)((char*)(r1)), 21); \
		MTC2(*(uint32_t*)((char*)(r2)), 22); \
		MTC2(*(uint32_t*)((char*)(r2)), 6); }

// mtc2 0, lwc2 1
#define gte_ldlv0( r0 ) \
	{	MTC2((*(uint16_t*)((char*)(r0)+4) << 16) | *(uint16_t*)((char*)(r0)));\
		MTC2(*(uint16_t*)((char*)(r0)+8) << 16); }

// mtc2 8
#define gte_lddp( r0 )	\
	{	MTC2(*(uint32_t*)(r0), 8);	}

// mtc2 8
#define gte_ldIR0( r0 )	\
	{	MTC2(*(uint32_t*)(r0), 8);	}

// ctc2 13 14 15
#define gte_ldbkdir( r0, r1, r2 ) \
	{	CTC2(r0, 13); \
		CTC2(r1, 14); \
		CTC2(r2, 15);	}

// mtc2 12,13,14
#define gte_ldsxy3( r0, r1, r2 )\
	{	MTC2(*(uint32_t*)(r0), 12);			\
		MTC2(*(uint32_t*)(r2), 14);			\
		MTC2(*(uint32_t*)(r1), 13);	}

// mtc2 12,13,14
#define gte_ldsxy3c( r0 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)+0), 12 );		\
		MTC2(*(uint32_t*)((char*)(r0)+4), 13 );		\
		MTC2(*(uint32_t*)((char*)(r0)+8), 14 );	}

// mtc2 17,18,19
#define gte_ldsz3( r0, r1, r2 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 17 );		\
		MTC2(*(uint32_t*)((char*)(r1)), 18 );		\
		MTC2(*(uint32_t*)((char*)(r2)), 19 );	}

// mtc2 16,17,18,19
#define gte_ldsz4( r0, r1, r2, r3 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 16 );		\
		MTC2(*(uint32_t*)((char*)(r1)), 17 );		\
		MTC2(*(uint32_t*)((char*)(r2)), 18 );		\
		MTC2(*(uint32_t*)((char*)(r3)), 19 );	}

// ctc2 0,2,4
#define gte_ldopv1( r0 ) \
	{	CTC2(*(uint32_t*)((char*)(r0)), 0);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 2);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 4);}

// lwc2 9,10,11
#define gte_ldopv2( r0 ) \
	{	MTC2(*(uint32_t*)((char*)(r0)), 9);\
		MTC2(*(uint32_t*)((char*)(r0)+4), 10);\
		MTC2(*(uint32_t*)((char*)(r0)+8), 11);}

// ctc2 26
#define gte_SetGeomScreen( r0 ) \
	{	CTC2(r0, 26);	}

// ctc2 24,25
#define gte_SetGeomOffset( r0, r1 )\
	{	CTC2(r0 << 16, 24); \
		CTC2(r1 << 16, 25);}

// ctc2 13,14,15
#define gte_SetBackColor( r0, r1, r2 ) \
	{	CTC2(r0 << 4, 13); \
		CTC2(r1 << 4, 14); \
		CTC2(r2 << 4, 15); }


// ctc2 21, 22, 23
#define gte_SetFarColor( r0, r1, r2 ) \
	{	CTC2(r0 << 4, 21); \
		CTC2(r1 << 4, 22); \
		CTC2(r2 << 4, 23); }

 // load ctc2 8-11
#define gte_SetLightMatrix( r0 ) \
	{	CTC2(*(uint32_t*)((char*)(r0)), 8);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 9);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 10);\
		CTC2(*(uint32_t*)((char*)(r0)+12), 11);\
		CTC2(*(uint32_t*)((char*)(r0)+16), 12);}

// load ctc2 16-20
#define gte_SetColorMatrix( r0 ) \
	{	CTC2(*(uint32_t*)((char*)(r0)), 16);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 17);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 18);\
		CTC2(*(uint32_t*)((char*)(r0)+12), 19);\
		CTC2(*(uint32_t*)((char*)(r0)+16), 20);}

// load ctc2 0-4
#define gte_SetRotMatrix( r0 )	\
	{	CTC2(*(uint32_t*)((char*)(r0)), 0);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 1);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 2);\
		CTC2(*(uint32_t*)((char*)(r0)+12), 3);\
		CTC2(*(uint32_t*)((char*)(r0)+16), 4);}

// load ctc2 5-7
#define gte_SetTransVector( r0 )\
	{	CTC2_S(*(int*)((char*)(r0)+0), 5);\
		CTC2_S(*(int*)((char*)(r0)+4), 6);\
		CTC2_S(*(int*)((char*)(r0)+8), 7);}

// load ctc2 5-7
#define gte_SetTransMatrix( r0 ) \
	{	CTC2(*(uint32_t*)((char*)(r0)+20), 5);\
		CTC2(*(uint32_t*)((char*)(r0)+24), 6);\
		CTC2(*(uint32_t*)((char*)(r0)+28), 7);}

// ctc2 8-12
#define gte_SetLightMatrix( r0 )\
	{	CTC2(*(uint32_t*)((char*)(r0)), 8);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 9);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 10);\
		CTC2(*(uint32_t*)((char*)(r0)+12), 11);\
		CTC2(*(uint32_t*)((char*)(r0)+16), 12);}

// ctc2 16-20
#define gte_SetColorMatrix( r0 )\
	{	CTC2(*(uint32_t*)((char*)(r0)), 16);\
		CTC2(*(uint32_t*)((char*)(r0)+4), 17);\
		CTC2(*(uint32_t*)((char*)(r0)+8), 18);\
		CTC2(*(uint32_t*)((char*)(r0)+12), 19);\
		CTC2(*(uint32_t*)((char*)(r0)+16), 20);}

// mtc2 9,10,11
#define gte_ldopv2SV( r0 ) \
	{	MTC2(*(uint16_t*)((char*)(r0)), 9);\
		MTC2(*(uint16_t*)((char*)(r0) + 2), 10);\
		MTC2(*(uint16_t*)((char*)(r0) + 4), 11);}

// ctc2 0,2,4
#define gte_ldopv1SV( r0 )\
	{	CTC2(*(uint16_t*)((char*)(r0)), 0);\
		CTC2(*(uint16_t*)((char*)(r0) + 2), 2);\
		CTC2(*(uint16_t*)((char*)(r0) + 4), 4);}

// mtc2 30
#define gte_ldlzc( r0 ) \
	MTC2(*(uint32_t*)((char*)(r0)), 30);

/*
 * Type 2 functions
 */

 // FIXME: there is lack of some macros
 // please refer to official Psy-Q PSX SDK headers
 // and this: https://github.com/ogamespec/pops-gte/blob/master/docs/gte.txt
 // and this: https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Inlinref.pdf

#define gte_nop()

#define gte_rtps() gte_cop2(0x0180001);

#define gte_rtpt() gte_cop2(0x0280030);

#define gte_rt() gte_cop2(0x0480012);

#define gte_rtv0() gte_cop2(0x0486012);

#define gte_rtv1() gte_cop2(0x048E012);

#define gte_rtv2() gte_cop2(0x0496012);

#define gte_rtir() gte_cop2(0x049E012);

#define gte_rtir_sf0() gte_cop2(0x041E012);

#define gte_rtv0tr() gte_cop2(0x0480012);

#define gte_rtv1tr() gte_cop2(0x0488012);

#define gte_rtv2tr() gte_cop2(0x0490012);

#define gte_rtirtr() op2 0x0498012);

#define gte_rtv0bk() gte_cop2(0x0482012);

#define gte_rtv1bk() gte_cop2(0x048A012);

#define gte_rtv2bk() gte_cop2(0x0492012);

#define gte_rtirbk() gte_cop2(0x049A012);

#define gte_ll() gte_cop2(0x04A6412);

#define gte_llv0() gte_cop2(0x04A6012);

#define gte_llv1() gte_cop2(0x04AE012);

#define gte_llv2() gte_cop2(0x04B6012);

#define gte_llir() gte_cop2(0x04BE012);

#define gte_llv0tr() gte_cop2(0x04A0012);

#define gte_llv1tr() gte_cop2(0x04A8012);

#define gte_llv2tr() gte_cop2(0x04B0012);

#define gte_llirtr() gte_cop2(0x04B8012);

#define gte_llv0bk() gte_cop2(0x04A2012);

#define gte_llv1bk() gte_cop2(0x4AA012);

#define gte_llv2bk() gte_cop2(0x04B2012);

#define gte_llirbk() gte_cop2(0x04BA012);

#define gte_lc() gte_cop2(0x04DA412);

#define gte_lcv0() gte_cop2(0x04C6012);

#define gte_lcv1() gte_cop2(0x04CE012);

#define gte_lcv2() gte_cop2(0x04D6012);

#define gte_lcir() gte_cop2(0x04DE012);

#define gte_lcv0tr() gte_cop2(0x04C0012);

#define gte_lcv1tr() gte_cop2(0x04C8012);

#define gte_lcv2tr() gte_cop2(0x04D0012);

#define gte_lcirtr() gte_cop2(0x04D8012);

#define gte_lcv0bk() gte_cop2(0x04C2012);

#define gte_lcv1bk() gte_cop2(0x04CA012);

#define gte_lcv2bk() gte_cop2(0x04D2012);

#define gte_lcirbk() gte_cop2(0x04DA012);

#define gte_dpcl() gte_cop2(0x0680029);

#define gte_dpcs() gte_cop2(0x0780010);

#define gte_dpct() gte_cop2(0x0F8002A);

#define gte_intpl() gte_cop2(0x0980011);

#define gte_sqr12() gte_cop2(0x0A80428);

#define gte_sqr0() gte_cop2(0x0A00428);

#define gte_ncs() gte_cop2(0x0C8041E);

#define gte_nct() gte_cop2(0x0D80420);

#define gte_ncds() gte_cop2(0x0E80413);

#define gte_ncdt() gte_cop2(0x0F80416);

#define gte_nccs() gte_cop2(0x0108041B);

#define gte_ncct() gte_cop2(0x0118043F);

#define gte_cdp() gte_cop2(0x01280414);

#define gte_cc() gte_cop2(0x0138041C);

#define gte_nclip() gte_cop2(0x01400006);

#define gte_avsz3() gte_cop2(0x0158002D);

#define gte_avsz4() gte_cop2(0x0168002E);

#define gte_op12() gte_cop2(0x0178000C);

#define gte_op0() gte_cop2(0x0170000C);

#define gte_gpf12() gte_cop2(0x0198003D);

#define gte_gpf0() gte_cop2(0x0190003D);

#define gte_gpl12() gte_cop2(0x01A8003E);

#define gte_gpl0() gte_cop2(0x01A0003E);

#define gte_mvmva_core( r0 ) gte_cop2(r0)

#define gte_mvmva(sf,mx,v,cv,lm) gte_mvmva_core( 0x0400012 | \
	((sf)<<19) | ((mx)<<17) | ((v)<<15) | ((cv)<<13) | ((lm)<<10) )


/*
 * Type 2 functions without nop
 */


#define gte_rtps_b() gte_cop2(0x0180001);

#define gte_rtpt_b() gte_cop2(0x0280030);

#define gte_rt_b() gte_cop2(0x0480012);

#define gte_rtv0_b() gte_cop2(0x0486012);

#define gte_rtv1_b() gte_cop2(0x048E012);

#define gte_rtv2_b() gte_cop2(0x0496012);

#define gte_rtir_b() gte_cop2(0x049E012);

#define gte_rtir_sf0_b() gte_cop2(0x041E012);

#define gte_rtv0tr_b() gte_cop2(0x0480012);

#define gte_rtv1tr_b() gte_cop2(0x0488012);

#define gte_rtv2tr_b() gte_cop2(0x0490012);

#define gte_rtirtr_b() gte_cop2(0x0498012);

#define gte_rtv0bk_b() gte_cop2(0x0482012);

#define gte_rtv1bk_b() gte_cop2(0x048A012);

#define gte_rtv2bk_b() gte_cop2(0x0492012);

#define gte_rtirbk_b() gte_cop2(0x049A012);

#define gte_ll_b() gte_cop2(0x04A6412);

#define gte_llv0_b() gte_cop2(0x04A6012);

#define gte_llv1_b() gte_cop2(0x04AE012);

#define gte_llv2_b() gte_cop2(0x04B6012);

#define gte_llir_b() gte_cop2(0x04BE012);

#define gte_llv0tr_b() gte_cop2(0x04A0012);

#define gte_llv1tr_b() gte_cop2(0x04A8012);

#define gte_llv2tr_b() gte_cop2(0x04B0012);

#define gte_llirtr_b() gte_cop2(0x04B8012);

#define gte_llv0bk_b() gte_cop2(0x04A2012);

#define gte_llv1bk_b() gte_cop2(0x4AA012);

#define gte_llv2bk_b() gte_cop2(0x04B2012);

#define gte_llirbk_b() gte_cop2(0x04BA012);

#define gte_lc_b() gte_cop2(0x04DA412);

#define gte_lcv0_b() gte_cop2(0x04C6012);

#define gte_lcv1_b() gte_cop2(0x04CE012);

#define gte_lcv2_b() gte_cop2(0x04D6012);

#define gte_lcir_b() gte_cop2(0x04DE012);

#define gte_lcv0tr_b() gte_cop2(0x04C0012);

#define gte_lcv1tr_b() gte_cop2(0x04C8012);

#define gte_lcv2tr_b() gte_cop2(0x04D0012);

#define gte_lcirtr_b() gte_cop2(0x04D8012);

#define gte_lcv0bk_b() gte_cop2(0x04C2012);

#define gte_lcv1bk_b() gte_cop2(0x04CA012);

#define gte_lcv2bk_b() gte_cop2(0x04D2012);

#define gte_lcirbk_b() gte_cop2(0x04DA012);

#define gte_dpcl_b() gte_cop2(0x0680029);

#define gte_dpcs_b() gte_cop2(0x0780010);

#define gte_dpct_b() gte_cop2(0x0F8002A);

#define gte_intpl_b() gte_cop2(0x0980011);

#define gte_sqr12_b() gte_cop2(0x0A80428);

#define gte_sqr0_b() gte_cop2(0x0A00428);

#define gte_ncs_b() gte_cop2(0x0C8041E);

#define gte_nct_b() gte_cop2(0x0D80420);

#define gte_ncds_b() gte_cop2(0x0E80413);

#define gte_ncdt_b() gte_cop2(0x0F80416);

#define gte_nccs_b() gte_cop2(0x0108041B);

#define gte_ncct_b() gte_cop2(0x0118043F);

#define gte_cdp_b() gte_cop2(0x01280414);

#define gte_cc_b() gte_cop2(0x0138041C);

#define gte_nclip_b() gte_cop2(0x01400006);

#define gte_avsz3_b() gte_cop2(0x0158002D);

#define gte_avsz4_b() gte_cop2(0x0168002E);

#define gte_op12_b() gte_cop2(0x0178000C);

#define gte_op0_b() gte_cop2(0x0170000C);

#define gte_gpf12_b() gte_cop2(0x0198003D);

#define gte_gpf0_b() gte_cop2(0x0190003D);

#define gte_gpl12_b() gte_cop2(0x01A8003E);

#define gte_gpl0_b() gte_cop2(0x01A0003E);

#define gte_mvmva_core_b( r0 ) gte_cop2(r0)

#define gte_mvmva_b(sf,mx,v,cv,lm) gte_mvmva_core_b( 0x0400012 | \
	((sf)<<19) | ((mx)<<17) | ((v)<<15) | ((cv)<<13) | ((lm)<<10) )


 /*
  * Type 3 functions
  */

  // store mfc2 9,10,11
#define gte_stclmv( r0 ) \
	{	*(uint16_t*)((char*)(r0)) = MFC2(9) & 0xFFFF; \
		*(uint16_t*)((char*)(r0)+6) = MFC2(10) & 0xFFFF; \
		*(uint16_t*)((char*)(r0)+12) = MFC2(11) & 0xFFFF;}

#if USE_PGXP

// swc2 14
#define gte_stsxy( r0 ) \
	{*(uint32_t*)((char*)(r0)) = *(uint32_t*)&g_FP_SXYZ2.x;}

// mfc2 12-14
#define gte_stsxy3( r0, r1, r2 )	\
	{	*(uint32_t*)((char*)(r0)) = *(uint32_t*)&g_FP_SXYZ0.x;\
		*(uint32_t*)((char*)(r1)) = *(uint32_t*)&g_FP_SXYZ1.x;\
		*(uint32_t*)((char*)(r2)) = *(uint32_t*)&g_FP_SXYZ2.x;}

// swc2 14
#define gte_stsxy2( r0 ) gte_stsxy(r0)

// swc2 13
#define gte_stsxy1( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = *(uint32_t*)&g_FP_SXYZ1.x;}

// swc2 12
#define gte_stsxy0( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = *(uint32_t*)&g_FP_SXYZ0.x;}

#else

// swc2 14
#define gte_stsxy( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(14);}

// mfc2 12-14
#define gte_stsxy3( r0, r1, r2 )	\
	{	*(uint32_t*)((char*)(r0)) = MFC2(12);\
		*(uint32_t*)((char*)(r1)) = MFC2(13);\
		*(uint32_t*)((char*)(r2)) = MFC2(14);}

// swc2 14
#define gte_stsxy2( r0 ) gte_stsxy(r0)

// swc2 13
#define gte_stsxy1( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(13);}

// swc2 12
#define gte_stsxy0( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(12);}

#endif // USE_PGXP

// swc2 8
#define gte_stdp( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(8);}

// cfc2 31
#define gte_stflg( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = CFC2(31);}

// swc2 7
#define gte_stotz( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(7);}

// mfc2 19 
#define gte_stszotz( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(19) >> 2;}

// swc2 24
#define gte_stopz( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(24);}

// swc2 19
#define gte_stsz( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(19);}

// swc2 9-11
#define gte_stlvl( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(9); \
		*(uint32_t*)((char*)(r0)+4) = MFC2(10);\
		*(uint32_t*)((char*)(r0)+8) = MFC2(11);}

// mfc2 9, 10, 11
#define gte_stsv( r0 ) \
	{	*(uint16_t*)((char*)(r0)) = MFC2(9) & 0xFFFF; \
		*(uint16_t*)((char*)(r0)+2) = MFC2(10) & 0xFFFF;\
		*(uint16_t*)((char*)(r0)+4) = MFC2(11) & 0xFFFF;}

// mfc2 9,10
#define gte_stbv( r0 ) \
	{	*((char*)(r0)) = MFC2(9) & 0xFF; \
		*((char*)(r0)+1) = MFC2(10) & 0xFF;}

// mfc2 9,10,11
#define gte_stcv( r0 ) \
	{	*((char*)(r0)) = MFC2(9) & 0xFF; \
		*((char*)(r0)+1) = MFC2(10) & 0xFF;\
		*((char*)(r0)+2) = MFC2(11) & 0xFF;}

// swc2 17,18,19
#define gte_stsz3( r0, r1, r2 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(17);\
		*(uint32_t*)((char*)(r1)) = MFC2(18);\
		*(uint32_t*)((char*)(r2)) = MFC2(19);}

// swc2 16,17,18,19
#define gte_stsz4( r0, r1, r2, r3 ) \
	{	*(uint32_t*)((char*)(r0)) = MFC2(16);\
		*(uint32_t*)((char*)(r1)) = MFC2(17);\
		*(uint32_t*)((char*)(r2)) = MFC2(18);\
		*(uint32_t*)((char*)(r3)) = MFC2(19);}

// swc2 25,26,27
#define gte_stlvnl( r0 ) \
	{	*(uint32_t*)((char*)(r0)+0) = MFC2(25);\
		*(uint32_t*)((char*)(r0)+4) = MFC2(26);\
		*(uint32_t*)((char*)(r0)+8) = MFC2(27);}

// ctc2 5,6,7
#define gte_ldtr( r0, r1, r2 ) \
	{	CTC2(r0, 5);\
		CTC2(r1, 6);\
		CTC2(r2, 7);}

// cfc2 5,6,7
#define gte_sttr( r0 ) \
	{	*(uint32_t*)((char*)(r0)+0) = CFC2(5);\
		*(uint32_t*)((char*)(r0)+4) = CFC2(6);\
		*(uint32_t*)((char*)(r0)+8) = CFC2(7);}

// swc2 22
#define gte_strgb( r0 )\
	{	*(uint32_t*)((char*)(r0)) = MFC2(22);}

// swc2 20 21 22
#define gte_strgb3( r0, r1, r2 )\
	{	*(uint32_t*)((char*)(r0)) = MFC2(20);\
		*(uint32_t*)((char*)(r1)) = MFC2(21);\
		*(uint32_t*)((char*)(r2)) = MFC2(22);}


// store cfc2 0-7
#define gte_ReadRotMatrix( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = CFC2(0);		\
		*(uint32_t*)((char*)(r0)+4) = CFC2(1);	\
		*(uint32_t*)((char*)(r0)+8) = CFC2(2);	\
		*(uint32_t*)((char*)(r0)+12) = CFC2(3);	\
		*(uint32_t*)((char*)(r0)+16) = CFC2(4);	\
		*(uint32_t*)((char*)(r0)+20) = CFC2(5);	\
		*(uint32_t*)((char*)(r0)+24) = CFC2(6);	\
		*(uint32_t*)((char*)(r0)+28) = CFC2(7);}

// cfc2 16-23
#define gte_ReadColorMatrix( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = CFC2(16);		\
		*(uint32_t*)((char*)(r0)+4) = CFC2(17);	\
		*(uint32_t*)((char*)(r0)+8) = CFC2(18);	\
		*(uint32_t*)((char*)(r0)+12) = CFC2(19);	\
		*(uint32_t*)((char*)(r0)+16) = CFC2(20);	\
		*(uint32_t*)((char*)(r0)+20) = CFC2(21);	\
		*(uint32_t*)((char*)(r0)+24) = CFC2(22);	\
		*(uint32_t*)((char*)(r0)+28) = CFC2(23);}

// cfc2 8-15
#define gte_ReadLightMatrix( r0 ) \
	{	*(uint32_t*)((char*)(r0)) = CFC2(8);		\
		*(uint32_t*)((char*)(r0)+4) = CFC2(9);	\
		*(uint32_t*)((char*)(r0)+8) = CFC2(10);	\
		*(uint32_t*)((char*)(r0)+12) = CFC2(11);	\
		*(uint32_t*)((char*)(r0)+16) = CFC2(12);	\
		*(uint32_t*)((char*)(r0)+20) = CFC2(13);	\
		*(uint32_t*)((char*)(r0)+24) = CFC2(14);	\
		*(uint32_t*)((char*)(r0)+28) = CFC2(15);}

// swc2 31
#define gte_stlzc( r0 ) \
	*(uint16_t*)((char*)(r0)) = MFC2(31);

// cfc2 21,22,23
#define gte_stfc( r0 )\
	{	*(uint32_t*)((char*)(r0)+0) = CFC2(21);\
		*(uint32_t*)((char*)(r0)+4) = CFC2(22);\
		*(uint32_t*)((char*)(r0)+8) = CFC2(23);}