/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_OPS_CARITH_H
#define __VASP_OPS_CARITH_H

#include "opfuns.h"

// Arithmetic math functions

DEFOP(S,d_cadd,add,cbin)
DEFOP(S,d_csub,sub,cbin)
DEFOP(S,d_csubr,subr,cbin)
DEFOP(S,d_cmul,mul,cbin)
DEFOP(S,d_cdiv,div,cbin)
DEFOP(S,d_cdivr,divr,cbin)

DEFOP(S,d_csqr,sqr,cun)

DEFOP(S,d_cpowi,powi,cop)

DEFOP(S,d_cabs,abs,cun)


namespace VaspOp {
	inline Vasp *m_cadd(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cadd); }  // complex add (pairs of vecs or complex)
	inline Vasp *m_csub(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_csub); }  // complex sub (pairs of vecs or complex)
	inline Vasp *m_csubr(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_csubr); }  // reverse complex sub (pairs of vecs or complex)
	inline Vasp *m_cmul(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cmul); }  // complex mul (pairs of vecs or complex)
	inline Vasp *m_cdiv(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cdiv); }  // complex div (pairs of vecs or complex)
	inline Vasp *m_cdivr(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cdivr); }  // complex reverse div (pairs of vecs or complex)

	inline Vasp *m_csqr(OpParam &p,CVasp &src,CVasp *dst = NULL) { return m_cun(p,src,dst,VecOp::d_csqr); }  // complex square (with each two channels)
//	inline Vasp *m_csqrt(OpParam &p,CVasp &src,CVasp *dst = NULL) { return m_cun(p,src,dst,VecOp::d_csqrt); }  // complex square root (how about branches?)

	Vasp *m_cpowi(OpParam &p,CVasp &src,const Argument &arg,CVasp *dst = NULL); // complex integer power (with each two channels)

	inline Vasp *m_cabs(OpParam &p,CVasp &src,CVasp *dst = NULL) { return m_cun(p,src,dst,VecOp::d_cabs); }  // absolute values
}

#endif