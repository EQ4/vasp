/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_OPS_ARITH_H
#define __VASP_OPS_ARITH_H

// Arithmetic math functions

namespace VecOp {
	BL d_add(OpParam &p); 
	BL d_sub(OpParam &p); 
	BL d_subr(OpParam &p); 
	BL d_mul(OpParam &p); 
	BL d_div(OpParam &p); 
	BL d_divr(OpParam &p); 
	BL d_mod(OpParam &p); 

	BL d_cadd(OpParam &p); 
	BL d_csub(OpParam &p); 
	BL d_csubr(OpParam &p); 
	BL d_cmul(OpParam &p); 
	BL d_cdiv(OpParam &p); 
	BL d_cdivr(OpParam &p); 

	BL d_sign(OpParam &p); 
	BL d_abs(OpParam &p); 
	BL d_cabs(OpParam &p); 
}

namespace VaspOp {
	inline Vasp *m_add(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_add); } // add to (one vec or real)
	inline Vasp *m_sub(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_sub); } // sub from (one vec or real)
	inline Vasp *m_subr(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_subr); } // reverse sub from (one vec or real)
	inline Vasp *m_mul(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_mul); } // mul with (one vec or real)
	inline Vasp *m_div(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_div); } // div by (one vec or real)
	inline Vasp *m_divr(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_divr); } // reverse div by (one vec or real)
	inline Vasp *m_mod(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(p,src,arg,dst,VecOp::d_mod); } // modulo by (one vec or real)

	inline Vasp *m_cadd(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cadd); }  // complex add (pairs of vecs or complex)
	inline Vasp *m_csub(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_csub); }  // complex sub (pairs of vecs or complex)
	inline Vasp *m_csubr(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_csubr); }  // reverse complex sub (pairs of vecs or complex)
	inline Vasp *m_cmul(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cmul); }  // complex mul (pairs of vecs or complex)
	inline Vasp *m_cdiv(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cdiv); }  // complex div (pairs of vecs or complex)
	inline Vasp *m_cdivr(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(p,src,arg,dst,VecOp::d_cdivr); }  // complex reverse div (pairs of vecs or complex)

	inline Vasp *m_sign(OpParam &p,Vasp &src,Vasp *dst = NULL) { return m_run(p,src,dst,VecOp::d_sign); }  // sign function 
	inline Vasp *m_abs(OpParam &p,Vasp &src,Vasp *dst = NULL) { return m_run(p,src,dst,VecOp::d_abs); }  // absolute values
	inline Vasp *m_cabs(OpParam &p,Vasp &src,Vasp *dst = NULL) { return m_cun(p,src,dst,VecOp::d_cabs); }  // absolute values
}

#endif
