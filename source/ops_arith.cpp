/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "ops_arith.h"
#include "opdefs.h"
#include "util.h"
#include <math.h>

template<class T> inline V f_radd(T &v,T a,T b) { v = a+b; }
template<class T> inline V f_rsub(T &v,T a,T b) { v = a-b; }
template<class T> inline V f_rsubr(T &v,T a,T b) { v = b-a; }
template<class T> inline V f_rmul(T &v,T a,T b) { v = a*b; }
template<class T> inline V f_rdiv(T &v,T a,T b) { v = a/b; }
template<class T> inline V f_rdivr(T &v,T a,T b) { v = b/a; }
template<class T> inline V f_rmod(T &v,T a,T b) { v = fmod(a,b); }

BL VecOp::d_add(OpParam &p) { D__rbin(f_radd<S>,p); }
BL VecOp::d_sub(OpParam &p) { D__rbin(f_rsub<S>,p); }
BL VecOp::d_subr(OpParam &p) { D__rbin(f_rsubr<S>,p); }
BL VecOp::d_mul(OpParam &p) { D__rbin(f_rmul<S>,p); }
BL VecOp::d_div(OpParam &p) { D__rbin(f_rdiv<S>,p); }
BL VecOp::d_divr(OpParam &p) { D__rbin(f_rdivr<S>,p); }
BL VecOp::d_mod(OpParam &p) { D__rbin(f_rmod<S>,p); }


VASP_BINARY("vasp.+",add,true,VASP_ARG_R(0),"adds a value or vasp")
VASP_BINARY("vasp.-",sub,true,VASP_ARG_R(0),"subtracts a value or vasp")
VASP_BINARY("vasp.!-",subr,true,VASP_ARG_R(0),"reverse subtracts a value or vasp")
VASP_BINARY("vasp.*",mul,true,VASP_ARG_R(1),"multiplies by a value or vasp")
VASP_BINARY("vasp./",div,true,VASP_ARG_R(1),"divides by a value or vasp")
VASP_BINARY("vasp.!/",divr,true,VASP_ARG_R(1),"reverse divides by a value or vasp")
VASP_BINARY("vasp.%",mod,true,VASP_ARG_R(0),"calculates the remainder of the division by a value or vasp")

// -----------------------------------------------------

template<class T> inline V f_rsqr(T &v,T a) { v = a*a; } 
template<class T> inline V f_rssqr(T &v,T a) { v = a*fabs(a); } 

BL VecOp::d_sqr(OpParam &p) { D__run(f_rsqr<S>,p); }
BL VecOp::d_ssqr(OpParam &p) { return d__run(f_rssqr<S>,p); }

VASP_UNARY("vasp.sqr",sqr,true,"") 
VASP_UNARY("vasp.ssqr",ssqr,true,"") 


// -----------------------------------------------------

template<class T> inline V f_rsign(T &v,T a) { v = (a == 0?0:(a < 0?-1.:1.)); }  
template<class T> inline V f_rabs(T &v,T a) { v = fabs(a); }  

BL VecOp::d_sign(OpParam &p) { D__run(f_rsign<S>,p); }
BL VecOp::d_abs(OpParam &p) { D__run(f_rabs<S>,p); }


VASP_UNARY("vasp.sign",sign,true,"") 
VASP_UNARY("vasp.abs",abs,true,"") 

