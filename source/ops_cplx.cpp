/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "ops_cplx.h"
#include "opdefs.h"


template<class T> V f_radd(T &rv,T &iv,T ra,T ia,T rb,T) 
{ 
	register const R _abs = sqrt(sqabs(ra,ia))+rb;
	register const R _phi = arg(ra,ia);

	rv = _abs*cos(_phi),iv = _abs*sin(_phi);
} 

BL VecOp::d_radd(OpParam &p) { return d__cbin(f_radd<S>,p); }

Vasp *VaspOp::m_radd(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst) 
{ 
	Vasp *ret = NULL;
	CVecBlock *vecs = GetCVecs(p.opname,src,dst);
	if(vecs) {
		if(arg.IsList() && arg.GetList().Count() >= 1 && flx::CanbeFloat(arg.GetList()[0]))
			p.cbin.rarg = flx::GetAFloat(arg.GetList()[0]);
		else {
			post("%s - argument is invalid -> set to 0",p.opname);
			p.cbin.rarg = 0;
		}
		p.cbin.iarg = 0; // not used anyway

		ret = DoOp(vecs,VecOp::d_radd,p);
		delete vecs;
	}
	return ret;
}




template<class T> V f_cnorm(T &rv,T &iv,T ra,T ia) 
{ 
	register T f = sqabs(ra,ia);
	if(f) { f = 1./sqrt(f); rv = ra*f,iv = ia*f; }
	else rv = iv = 0;
}

BL VecOp::d_cnorm(OpParam &p) { return d__cun(f_cnorm<S>,p); }


template<class T> V f_polar(T &rv,T &iv,T ra,T ia) { rv = sqrt(sqabs(ra,ia)),iv = arg(ra,ia); }
template<class T> V f_rect(T &rv,T &iv,T ra,T ia) { rv = ra*cos(ia),iv = ra*sin(ia); }

BL VecOp::d_polar(OpParam &p) { return d__cun(f_polar<S>,p); }
BL VecOp::d_rect(OpParam &p) { return d__cun(f_rect<S>,p); }


template<class T> inline V f_cconj(T &,T &iv,T,T ia) { iv = -ia; }

BL VecOp::d_cconj(OpParam &p) { D__cun(f_cconj<S>,p); }


//template<class T> inline V f_cswap(T &rv,T &iv,T ra,T ia) { rv = ia,iv = ra; }
//BL VecOp::d_cswap(OpParam &p) { D__cun(f_cswap<S>,p); }
