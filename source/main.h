/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_H
#define __VASP_H

#define VASP_VERSION "0.0.1"


#include <flext.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 203)
#error You need at least flext version 0.2.3
#endif

#include <typeinfo.h>

#define I int
#define L long
#define F float
#define D double
#define C char
#define BL bool
#define V void
#define S t_sample // type for samples
#define R double // type for internal calculations
#define CX complex
#define VX vector

class complex 
{ 
public:
	complex() {}
	complex(F re,F im = 0): real(re),imag(im) {}

	F real,imag; 
};

class vector 
{ 
public:
	vector(): dim(0),data(NULL) {}
	~vector() { if(data) delete[] data; }

	I Dim() const { return dim; }
	F *Data() { return data; }
	const F *Data() const { return data; }
protected:
	I dim; F *data; 
};

#ifndef PI
#define PI 3.1415926535897932385
#endif



class VBuffer:
	public flext_base::buffer
{
	typedef flext_base::buffer parent;

public:
	VBuffer(t_symbol *s = NULL,I chn = 0,I len = -1,I offs = 0);
	VBuffer(const VBuffer &v);
	~VBuffer();

	VBuffer &operator =(const VBuffer &v);
	VBuffer &Set(t_symbol *s = NULL,I chn = 0,I len = -1,I offs = 0);

	I Channel() const { return chn; }
	I Offset() const { return offs; }
	I Length() const { return len; }

	S *Pointer() { return Data()+Offset()*Channels()+Channel(); }

protected:
	I chn,offs,len;
};


class AtomList
{
public:
	AtomList(I argc,t_atom *argv = NULL);
	~AtomList();

	I Count() const { return cnt; }
	t_atom &operator [](I ix) { return lst[ix]; }
	const t_atom &operator [](I ix) const { return lst[ix]; }

	t_atom *Atoms() { return lst; }
	const t_atom *Atoms() const { return lst; }
	
protected:
	I cnt;
	t_atom *lst;
};


class XAtomList:
	public AtomList
{
public:
	XAtomList(const t_symbol *h,I argc,t_atom *argv): AtomList(argc,argv),hdr(h) {}

	const t_symbol *Header() const { return hdr; }
	
protected:
	const t_symbol *hdr;
};



class Vasp;

class Argument
{
public:
	Argument();
	~Argument();
	
	Argument &Parse(I argc,t_atom *argv);
	Argument &Clear();
	Argument &ClearAll();

	Argument &Set(Vasp *v);
	Argument &Set(I argc,t_atom *argv);
	Argument &Set(I i);
	Argument &Set(F f);
	Argument &Set(D d);
	Argument &Set(F re,F im);
	Argument &Set(VX *vec);

	Argument *Next() { return nxt; }
	Argument &Next(I i);
	Argument &Add(Argument *a);

	Argument &Add(Vasp *v);
	Argument &Add(I argc,t_atom *argv);
	Argument &Add(I i);
	Argument &Add(F f);
	Argument &Add(D d);
	Argument &Add(F re,F im);
	Argument &Add(VX *vec);

	BL IsNone() const { return tp == tp_none; }
	BL IsVasp() const { return tp == tp_vasp; }
	BL IsList() const { return tp == tp_list; }
	BL IsInt() const { return tp == tp_int; }
	BL CanbeInt() const { return tp == tp_int || tp == tp_float || tp_double; }
	BL IsFloat() const { return tp == tp_float; }
	BL CanbeFloat() const { return tp == tp_float || tp_double || tp == tp_int; }
	BL IsDouble() const { return tp == tp_double; }
	BL CanbeDouble() const { return tp == tp_double || tp_float || tp == tp_int; }
	BL IsComplex() const { return tp == tp_cx; }
	BL CanbeComplex() const { return tp == tp_cx || CanbeFloat(); }
	BL IsVector() const { return tp == tp_vx; }
	BL CanbeVector() const { return tp == tp_vx || CanbeComplex(); }

	const Vasp &GetVasp() const { return *dt.v; }
	const AtomList &GetList() const { return *dt.atoms; }
	I GetInt() const { return dt.i; }
	I GetAInt() const;
	F GetFloat() const { return dt.f; }
	F GetAFloat() const;
	D GetDouble() const { return dt.d; }
	D GetADouble() const;
	const CX &GetComplex() const { return *dt.cx; }
	CX GetAComplex() const;
	const VX &GetVector() const { return *dt.vx; }
	VX GetAVector() const;

protected:
	enum {
		tp_none,tp_vasp,tp_list,tp_int,tp_float,tp_double,tp_cx,tp_vx
	} tp;

	union {
		Vasp *v;
		AtomList *atoms;
		F f;
		D d;
		I i;
		CX *cx;
		VX *vx;
	} dt;

	Argument *nxt;
};


F arg(F re,F im);
inline F arg(const CX &c) { return arg(c.real,c.imag); }
inline F abs(F re,F im) { return re*re+im*im; }
inline F abs(const CX &c) { return abs(c.real,c.imag); }
inline F sgn(F x) { return x < 0.?-1.F:1.F; }
inline V swap(F &a,F &b) { F c = a; a = b; b = c; }


class VecBlock 
{
public:

	I Frames() const { return frms; }
	V Frames(I fr) { frms = fr; }

protected:
	VecBlock(I msrc,I marg,I mdst);
	~VecBlock();

	Vasp *_SrcVasp(I n);
	Vasp *_DstVasp(I n);
	Vasp *_ResVasp(I n); // either Dst or Src

	VBuffer *_Src(I ix) { return vecs[ix]; }
	VBuffer *_Arg(I ix) { return vecs[asrc+ix]; }
	VBuffer *_Dst(I ix) { return vecs[asrc+aarg+ix]; }
	V _Src(I ix,VBuffer *v) { vecs[ix] = v; }
	V _Arg(I ix,VBuffer *v) { vecs[asrc+ix] = v; }
	V _Dst(I ix,VBuffer *v) { vecs[asrc+aarg+ix] = v; }

private:
	I asrc,aarg,adst;
	VBuffer **vecs;
	I frms;
};


class RVecBlock:
	public VecBlock
{
public:
	RVecBlock(I _n,I _a = 0): VecBlock(_n,_a,_n),n(_n),a(_a) {}

	VBuffer *Src(I ix) { return _Src(ix); }
	VBuffer *Arg(I ix) { return _Arg(ix); }
	VBuffer *Dst(I ix) { return _Dst(ix); }
	V Src(I ix,VBuffer *v) { _Src(ix,v); }
	V Arg(I ix,VBuffer *v) { _Arg(ix,v); }
	V Dst(I ix,VBuffer *v) { _Dst(ix,v); }

	I Vecs() const { return n; }
	I Args() const { return a; }

	Vasp *SrcVasp() { return _SrcVasp(n); }
	Vasp *DstVasp() { return _DstVasp(n); }
	Vasp *ResVasp() { return _ResVasp(n); }

protected:
	I n,a;
};

class CVecBlock:
	public VecBlock
{
public:
	CVecBlock(I _np,I _ap = 0): VecBlock(_np*2,_ap*2,_np*2),np(_np),ap(_ap) {}

	VBuffer *ReSrc(I ix) { return _Src(ix*2); }
	VBuffer *ImSrc(I ix) { return _Src(ix*2+1); }
	VBuffer *ReArg(I ix) { return _Arg(ix*2); }
	VBuffer *ImArg(I ix) { return _Arg(ix*2+1); }
	VBuffer *ReDst(I ix) { return _Dst(ix*2); }
	VBuffer *ImDst(I ix) { return _Dst(ix*2+1); }
	V Src(I ix,VBuffer *vre,VBuffer *vim) { _Src(ix*2,vre); _Src(ix*2+1,vim); }
	V Arg(I ix,VBuffer *vre,VBuffer *vim) { _Arg(ix*2,vre); _Arg(ix*2+1,vim); }
	V Dst(I ix,VBuffer *vre,VBuffer *vim) { _Dst(ix*2,vre); _Dst(ix*2+1,vim); }

	I Pairs() const { return np; }
	I Args() const { return ap; }

	Vasp *SrcVasp() { return _SrcVasp(np*2); }
	Vasp *DstVasp() { return _DstVasp(np*2); }
	Vasp *ResVasp() { return _ResVasp(np*2); }

protected:
	I np,ap;
};


class OpParam {
public:
	OpParam(const C *opnm): opname(opnm),frames(0),rsdt(NULL),revdir(false) {}

	// check for overlap 
	// \remark if on same vector, stride is the same for src, arg, dst!
	inline BL SR_In() const { return rddt >= rsdt && rddt < rsdt+frames*rss; } 
	inline BL SI_In() const { return iddt >= isdt && iddt < isdt+frames*iss; } 
	inline BL AR_In() const { return rddt >= radt && rddt < radt+frames*ras; } 
	inline BL AI_In() const { return iddt >= iadt && iddt < iadt+frames*ias; } 
	
	// Can we reverse direction?
	inline BL SR_Can() const { return rsdt >= rddt && rsdt < rddt+frames*rds; } 
	inline BL SI_Can() const { return isdt >= iddt && isdt < iddt+frames*ids; } 
	inline BL AR_Can() const { return radt >= rddt && radt < rddt+frames*rds; } 
	inline BL AI_Can() const { return iadt >= iddt && iadt < iddt+frames*ids; } 
	
	// reverse direction
	inline V SR_Rev() { rsdt -= (frames-1)*(rss = -rss); }
	inline V SI_Rev() { isdt -= (frames-1)*(iss = -iss); }
	inline V AR_Rev() { radt -= (frames-1)*(ras = -ras); }
	inline V AI_Rev() { iadt -= (frames-1)*(ias = -ias); }
	inline V DR_Rev() { rddt -= (frames-1)*(rds = -rds); }
	inline V DI_Rev() { iddt -= (frames-1)*(ids = -ids); }
	
/*
	V SDR_Rev();
	V SDI_Rev();
	V SDC_Rev();
	V ADR_Rev();
	V ADI_Rev();
	V ADC_Rev();
	V SADR_Rev();
	V SADI_Rev();
	V SADC_Rev();
*/
	V R_Rev();
	V C_Rev();

	const C *opname;
	I frames;
	BL revdir;
	S *rsdt,*isdt; I rss,iss;
	S *rddt,*iddt; I rds,ids;
	S *radt,*iadt; I ras,ias;
	union {
		struct { R coef,carry; I rep; } flt;
		struct { R carry; I rep; } intdif;
		struct { I rep; } peaks;
		struct { R ph,phinc; } gen;
		struct { R factor,center; I mode; } tilt; 
		struct { R cur,inc; } bvl;
		struct { R sh; } sh;
		struct { I wndtp; } wnd;
		struct { R arg; } rbin; 
		struct { R rarg,iarg; } cbin; 
	};
};

namespace VecOp {
	typedef BL opfun(OpParam &p);

	BL d_copy(OpParam &p); 
	BL d_add(OpParam &p); 
	BL d_sub(OpParam &p); 
	BL d_mul(OpParam &p); 
	BL d_div(OpParam &p); 
	BL d_min(OpParam &p); 
	BL d_max(OpParam &p); 
	BL d_lwr(OpParam &p); 
	BL d_gtr(OpParam &p); 
	BL d_pow(OpParam &p); 

	BL d_ccopy(OpParam &p); 
	BL d_cadd(OpParam &p); 
	BL d_csub(OpParam &p); 
	BL d_cmul(OpParam &p); 
	BL d_cdiv(OpParam &p); 
	BL d_cmin(OpParam &p); 
	BL d_cmax(OpParam &p); 
	BL d_cpowi(OpParam &p); 

	BL d_sqr(OpParam &p); 
	BL d_ssqr(OpParam &p); 
	BL d_sqrt(OpParam &p); 
	BL d_ssqrt(OpParam &p); 
	BL d_exp(OpParam &p); 
	BL d_log(OpParam &p); 
	BL d_inv(OpParam &p); 
	BL d_abs(OpParam &p); 
	BL d_sign(OpParam &p); 
	BL d_opt(OpParam &p); 

	BL d_csqr(OpParam &p); 
	BL d_cinv(OpParam &p); 
	BL d_cabs(OpParam &p); 
	BL d_cswap(OpParam &p); 
	BL d_cconj(OpParam &p); 
	BL d_polar(OpParam &p); 
	BL d_cart(OpParam &p); 
	BL d_copt(OpParam &p); 
	BL d_cnorm(OpParam &p); 


	BL d_int(OpParam &p);
	BL d_dif(OpParam &p); 
	
	BL d_flp(OpParam &p);
	BL d_fhp(OpParam &p);
	
	BL d_tilt(OpParam &p);

	BL d_shift(OpParam &p);
	BL d_rot(OpParam &p);
	BL d_mirr(OpParam &p);
	
	BL d_osc(OpParam &p);
	BL d_cosc(OpParam &p);
	BL d_mosc(OpParam &p);
	BL d_mcosc(OpParam &p);
	BL d_phasor(OpParam &p);
	BL d_mphasor(OpParam &p);
	BL d_noise(OpParam &p);
	BL d_cnoise(OpParam &p);

	BL d_bevel(OpParam &p); 
	BL d_mbevel(OpParam &p); 

	BL d_window(OpParam &p);
	BL d_mwindow(OpParam &p);
	BL d_vwindow(OpParam &p);
	BL d_vmwindow(OpParam &p);
}

namespace VaspOp {
	typedef flext_base flx;

	inline I min(I a,I b) { return a < b?a:b; }
	inline I max(I a,I b) { return a > b?a:b; }

	RVecBlock *GetRVecs(const C *op,Vasp &src,Vasp *dst = NULL);
	CVecBlock *GetCVecs(const C *op,Vasp &src,Vasp *dst = NULL,BL full = false);
	RVecBlock *GetRVecs(const C *op,Vasp &src,const Vasp &arg,Vasp *dst = NULL,I multi = -1);
	CVecBlock *GetCVecs(const C *op,Vasp &src,const Vasp &arg,Vasp *dst = NULL,I multi = -1,BL full = false);
	
	Vasp *DoOp(RVecBlock *vecs,VecOp::opfun *fun,OpParam &p);
	Vasp *DoOp(CVecBlock *vecs,VecOp::opfun *fun,OpParam &p);

	// -------- transformations -----------------------------------

	// unary functions
	Vasp *m_run(Vasp &src,Vasp *dst,VecOp::opfun *fun,const C *opnm); // real unary (one vec or real)
	Vasp *m_cun(Vasp &src,Vasp *dst,VecOp::opfun *fun,const C *opnm); // complex unary (one vec or complex)
	// binary functions
	Vasp *m_rbin(Vasp &src,const Argument &arg,Vasp *dst,VecOp::opfun *fun,const C *opnm); // real binary (one vec or real)
	Vasp *m_cbin(Vasp &src,const Argument &arg,Vasp *dst,VecOp::opfun *fun,const C *opnm); // complex binary (one vec or complex)

	Vasp *m_copy(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_copy,"copy"); } // copy to (one vec or real)
	Vasp *m_add(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_add,"add"); } // add to (one vec or real)
	Vasp *m_sub(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_sub,"sub"); } // sub from (one vec or real)
	Vasp *m_mul(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_mul,"mul"); } // mul with (one vec or real)
	Vasp *m_div(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_div,"div"); } // div by (one vec or real)
	Vasp *m_min(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_min,"min"); } // min (one vec or real)
	Vasp *m_max(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_max,"max"); } // max (one vec or real)
	Vasp *m_lwr(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_lwr,"lwr"); } // lower than
	Vasp *m_gtr(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_gtr,"gtr"); } // greater than
	Vasp *m_pow(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_rbin(src,arg,dst,VecOp::d_pow,"pow"); } // power

	Vasp *m_ccopy(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_ccopy,"ccopy"); }  // complex copy (pairs of vecs or complex)
	Vasp *m_cadd(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cadd,"cadd"); }  // complex add (pairs of vecs or complex)
	Vasp *m_csub(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_csub,"csub"); }  // complex sub (pairs of vecs or complex)
	Vasp *m_cmul(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cmul,"cmul"); }  // complex mul (pairs of vecs or complex)
	Vasp *m_cdiv(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cdiv,"cdiv"); }  // complex div (pairs of vecs or complex)
	Vasp *m_cmin(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cmin,"cmin"); }  // complex (abs) min (pairs of vecs or complex)
	Vasp *m_cmax(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cmax,"cmax"); }  // complex (abs) max (pairs of vecs or complex)
	Vasp *m_cpowi(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_cbin(src,arg,dst,VecOp::d_cpowi,"cpowi"); }  // complex integer power (with each two channels)

	Vasp *m_minmax(Vasp &src,Vasp *dst = NULL,BL rev = false); // min/max 
	Vasp *m_maxmin(Vasp &src,Vasp *dst = NULL) { return m_minmax(src,dst,true); } // min/max 

	Vasp *m_sqr(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_sqr,"sqr"); }    // unsigned square 
	Vasp *m_ssqr(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_ssqr,"ssqr"); }   // signed square 
	Vasp *m_sqrt(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_sqrt,"sqrt"); }  // square root (from abs value)
	Vasp *m_ssqrt(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_ssqrt,"ssqrt"); }  // square root (from abs value)

	Vasp *m_csqr(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_csqr,"csqr"); }  // complex square (with each two channels)
//	Vasp *m_csqrt(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_csqrt); }  // complex square root (how about branches?)

	Vasp *m_exp(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_exp,"exp"); }  // exponential function
	Vasp *m_log(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_log,"log"); } // natural logarithm

//	Vasp *m_cexp(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cexp); }  // complex exponential function
//	Vasp *m_clog(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_clog); } // complex logarithm (how about branches?)

	Vasp *m_inv(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_inv,"inv"); }  // invert buffer values
	Vasp *m_cinv(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cinv,"cinv"); } // complex invert buffer values (each two)

	Vasp *m_abs(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_abs,"abs"); }  // absolute values
	Vasp *m_cabs(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cabs,"cabs"); }  // absolute values

	Vasp *m_sign(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_sign,"sign"); }  // sign function 

	Vasp *m_polar(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_polar,"polar"); } // cartesian -> polar (each two)
	Vasp *m_cart(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cart,"cart"); } // polar -> cartesian (each two)

	Vasp *m_opt(Vasp &src,Vasp *dst = NULL) { return m_run(src,dst,VecOp::d_opt,"opt"); }  // optimize
	Vasp *m_copt(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_copt,"copt"); }  // complex optimize

	Vasp *m_cnorm(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cnorm,"cnorm"); } // complex normalize

	Vasp *m_cswap(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cswap,"cswap"); }  // swap real and imaginary parts
	Vasp *m_cconj(Vasp &src,Vasp *dst = NULL) { return m_cun(src,dst,VecOp::d_cconj,"cconj"); }  // complex conjugate

	// int/dif functions
	Vasp *m_int(Vasp &src,const Argument &arg,Vasp *dst = NULL,BL inv = false); //! integrate
	Vasp *m_dif(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_int(src,arg,dst,true); } //! differentiate
	
	// extrema functions
	Vasp *m_peaks(Vasp &src,const Argument &arg,Vasp *dst = NULL,BL inv = false); //! find peaks
	Vasp *m_valleys(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_peaks(src,arg,dst,true); } //! find valleys	

	// Filter functions
	Vasp *m_fhp(Vasp &src,const Argument &arg,Vasp *dst = NULL,BL hp = true); //! hi pass
	Vasp *m_flp(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_fhp(src,arg,dst,false); } //! lo pass
	
	// Rearrange buffer 
	Vasp *m_shift(Vasp &src,const Argument &arg,Vasp *dst = NULL,BL sh = true,BL symm = false);  // shift buffer
	Vasp *m_xshift(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_shift(src,arg,dst,true,true); }  // shift buffer (symmetrically)
	Vasp *m_rot(Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_shift(src,arg,dst,false,false); } // rotate buffer
	Vasp *m_xrot(Vasp &src,const Argument &arg,Vasp *dst = NULL)  { return m_shift(src,arg,dst,false,true); }  // rotate buffer (symmetrically)
	Vasp *m_mirr(Vasp &src,Vasp *dst = NULL,BL symm = false);  //! mirror buffer
	Vasp *m_xmirr(Vasp &src,Vasp *dst = NULL) { return m_mirr(src,dst,true); } //! mirror buffer (symmetrically)

	// Generator functions 
	Vasp *m_osc(Vasp &src,const Argument &arg,Vasp *dst,BL mul = false);  // real osc
	Vasp *m_mosc(Vasp &src,const Argument &arg,Vasp *dst) { return m_osc(src,arg,dst,true); }   // * real osc
	Vasp *m_cosc(Vasp &src,const Argument &arg,Vasp *dst,BL mul = false);  // complex osc (phase rotates)
	Vasp *m_mcosc(Vasp &src,const Argument &arg,Vasp *dst) { return m_cosc(src,arg,dst,true); }  // * complex osc (phase rotates)
	Vasp *m_phasor(Vasp &src,const Argument &arg,Vasp *dst,BL mul = false);  // phasor
	Vasp *m_mphasor(Vasp &src,const Argument &arg,Vasp *dst) { return m_phasor(src,arg,dst,true); }  // * phasor
	Vasp *m_noise(Vasp &src,Vasp *dst);  // real noise
	Vasp *m_cnoise(Vasp &src,Vasp *dst); // complex noise (arg and abs random)
	Vasp *m_bevelup(Vasp &src,Vasp *dst,BL up = true,BL mul = false);  // bevel up 
	Vasp *m_mbevelup(Vasp &src,Vasp *dst) { return m_bevelup(src,dst,true,true); }   // * bevel up (fade in)
	Vasp *m_beveldn(Vasp &src,Vasp *dst) { return m_bevelup(src,dst,false,false); }  // bevel down
	Vasp *m_mbeveldn(Vasp &src,Vasp *dst) { return m_bevelup(src,dst,false,true); }   // * bevel down (fade out)
	Vasp *m_window(Vasp &src,const Argument &arg,Vasp *dst,BL mul = false);  // window curve
	Vasp *m_mwindow(Vasp &src,const Argument &arg,Vasp *dst) { return m_window(src,arg,dst,true); }  // * window curve

	// Resampling (around center sample)
	Vasp *m_tilt(Vasp &src,const Argument &arg,Vasp *dst = NULL,BL symm = false,I mode = 0); 
	// Symmetric resampling (around center sample)
	Vasp *m_xtilt(Vasp &src,const Argument &arg,Vasp *dst = NULL,I mode = 0) { return m_tilt(src,arg,dst,true,mode); }

	// Fourier transforms 
	Vasp *m_rfft(Vasp &src,Vasp *dst = NULL,BL inv = false);  // real forward
	Vasp *m_rifft(Vasp &src,Vasp *dst = NULL) { return m_rfft(src,dst,true); } // real inverse
	Vasp *m_cfft(Vasp &src,Vasp *dst = NULL,BL inv = false); // complex forward
	Vasp *m_cifft(Vasp &src,Vasp *dst = NULL) { return m_cfft(src,dst,true); } // complex inverse

}



class Vasp 
{
public:
	class Ref {
	public:
		Ref(): sym(NULL) {}
		Ref(VBuffer &b): sym(b.Symbol()),chn(b.Channel()),offs(b.Offset()) {}
		Ref(t_symbol *s,I c,I o): sym(s),chn(c),offs(o) {}

		V Clear() { sym = NULL; }
		BL Ok() const { return sym != NULL; }

		t_symbol *Symbol() const { return sym; }
		V Symbol(t_symbol *s) { sym = s; }
		I Channel() const { return chn; }
		V Channel(I c) { chn = c; }
		I Offset() const { return offs; }
		V Offset(I o) { offs = o; }
		V OffsetD(I o) { offs += o; }

	protected:
		t_symbol *sym;
		I chn;
		I offs; // counted in frames
	};

	Vasp();
	Vasp(I argc,t_atom *argv);
	Vasp(const Vasp &v);
	Vasp(I frames,const Ref &r);
	~Vasp();

	const C *thisName() const { return typeid(*this).name(); }

	Vasp &operator =(const Vasp &v);
	Vasp &operator ()(I argc,t_atom *argv /*,BL withvasp = false*/);

	// add another vector
	Vasp &operator +=(const Ref &r);
	// add vectors of another vasp
	Vasp &operator +=(const Vasp &v);

	// set used channels to 0
	Vasp &Clear() { frames = 0; chns = 0; return *this; }

	// used vectors
	I Vectors() const { return chns; }

	// length of the vasp (in frames)
	I Frames() const { return frames; }
	// set frame count
	V Frames(I fr) { frames = fr; }
	// set frame count differentially
	V FramesD(I frd) { frames += frd; }

	// set offset(s)
	V Offset(I fr);
	// set offset(s) differentially
	V OffsetD(I fr);

	BL Ok() const { return ref && Vectors() > 0; }
	BL IsComplex() const { return ref && Vectors() >= 2 && ref[1].Symbol() != NULL; }

	// get any vector - test if in range 0..Vectors()-1!
	const Ref &Vector(I ix) const { return ref[ix]; }
	Ref &Vector(I ix) { return ref[ix]; }

	// get real part - be sure that Ok!
	const Ref &Real() const { return Vector(0); }
	Ref &Real() { return Vector(0); }

	// get imaginary part - be sure that Complex!
	const Ref &Imag() const { return Vector(1); }
	Ref &Imag() { return Vector(1); }

	// get buffer associated to a channel
	VBuffer *Buffer(I ix) const;

	// Real/Complex
	VBuffer *ReBuffer() const { return Buffer(0); }
	VBuffer *ImBuffer() const { return Buffer(1); }

	// prepare and reference t_atom list for output
	AtomList *MakeList(BL withvasp = true);

	// make a graphical update of all buffers in vasp
	V Refresh();
	

#if 0

	// --------------------------------------------------------------------


	typedef BL (*argfunRA)(I,F *,I,Argument &);
	typedef BL (*argfunCA)(I,F *,I,F *,I,Argument &);
	
	typedef BL (*argfunR)(I,F *,I,F);
	typedef BL (*argfunC)(I,F *,I,F *,I,F,F);
	typedef BL (*argfunV)(I,F *,I,const F *,I);
	typedef BL (*argfunCV)(I,F *,I,F *,I,const F *,I,const F *,I);

	struct nop_funcs {
		argfunR funR;
		argfunC funC;
	};

	struct arg_funcs {
		argfunR funR;
		argfunC funC;
		argfunV funV;
		argfunCV funCV;
	};
#endif
protected:
	I frames; // length counted in frames
	I chns; // used channels
	I refs; // allocated channels (>= chns)
	Ref *ref;

	V Resize(I rcnt);

#if 0
private:
	typedef flext_base flx;

	Vasp *fr_nop(const C *op,F v,const nop_funcs &f) { return fr_arg(op,v,f.funR); }
	Vasp *fc_nop(const C *op,const CX &cx,const nop_funcs &f) { return fc_arg(op,cx,f.funC); }
	Vasp *fc_nop(const C *op,const Argument &params,const nop_funcs &f);

	Vasp *fr_prm(const C *op,argfunRA f,Argument &params);
	Vasp *fc_prm(const C *op,argfunCA f,Argument &params);
//	Vasp *fr_arg(const C *op,argfunVA f,Argument &a);
//	Vasp *fc_arg(const C *op,argfunCV f,Argument &a);
//	Vasp *fv_arg(const C *op,argfunV f,Argument &a);

	Vasp *fr_arg(const C *op,F v,argfunR f);
	Vasp *fr_arg(const C *op,F v,const arg_funcs &f) { return fr_arg(op,v,f.funR); }
	Vasp *fr_arg(const C *op,const Vasp &v,argfunV f);
	Vasp *fr_arg(const C *op,const Vasp &v,const arg_funcs &f) { return fr_arg(op,v,f.funV); }
	Vasp *fr_arg(const C *op,const Argument &arg,const arg_funcs &f);
	Vasp *fc_arg(const C *op,const CX &cx,argfunC f);
	Vasp *fc_arg(const C *op,const CX &cx,const arg_funcs &f) { return fc_arg(op,cx,f.funC); }
	Vasp *fc_arg(const C *op,const Vasp &v,argfunCV f);
	Vasp *fc_arg(const C *op,const Vasp &v,const arg_funcs &f) { return fc_arg(op,v,f.funCV); }
	Vasp *fc_arg(const C *op,const Argument &arg,const arg_funcs &f);
	Vasp *fv_arg(const C *op,const Vasp &v,argfunV f);
	Vasp *fv_arg(const C *op,const Vasp &v,const arg_funcs &f) { return fv_arg(op,v,f.funV); }
	Vasp *fv_arg(const C *op,const Argument &arg,const arg_funcs &f);
#endif
};


class vasp_base:
	public flext_base
{
	FLEXT_HEADER_S(vasp_base,flext_base,setup)

	enum xs_unit {
		xsu__ = -1,  // don't change
		xsu_sample = 0,xsu_buffer,xsu_ms,xsu_s
	};	

protected:
	vasp_base();
	virtual ~vasp_base();

	virtual V m_radio(I argc,t_atom *argv);  // commands for all

	virtual V m_argchk(BL chk);  // precheck argument on arrival
	virtual V m_loglvl(I lvl);  // noise level of log messages
	virtual V m_unit(xs_unit u);  // unit command

	BL refresh;  // immediate graphics refresh?
	BL argchk;   // pre-operation argument feasibility check
	xs_unit unit;  // time units
	I loglvl;	// noise level for log messages

	friend class Vasp;

	static const t_symbol *sym_vasp;
	static const t_symbol *sym_complex;
	static const t_symbol *sym_vector;
	static const t_symbol *sym_radio;

	BL ToOutVasp(I outlet,Vasp &v);

private:
	static V setup(t_class *);

	FLEXT_CALLBACK_G(m_radio)

	FLEXT_CALLBACK_B(m_argchk)
	FLEXT_CALLBACK_I(m_loglvl)
	FLEXT_CALLBACK_1(m_unit,xs_unit)
};


class vasp_op:
	public vasp_base
{
	FLEXT_HEADER(vasp_op,vasp_base)

protected:
	vasp_op();

	virtual V m_bang() = 0;						// do! and output current Vasp

	virtual V m_vasp(I argc,t_atom *argv); // trigger
	virtual I m_set(I argc,t_atom *argv);  // non trigger
	virtual V m_to(I argc,t_atom *argv); // set destinatioc

	virtual V m_update(I argc = 0,t_atom *argv = NULL);  // graphics update

	// destination vasp
	Vasp ref,dst;

	FLEXT_CALLBACK_G(m_to)
private:
	FLEXT_CALLBACK(m_bang)
	FLEXT_CALLBACK_G(m_vasp)
	FLEXT_CALLBACK_G(m_set)
	FLEXT_CALLBACK_G(m_update)
};



class vasp_tx:
	public vasp_op
{
	FLEXT_HEADER(vasp_tx,vasp_op)

protected:
	virtual V m_bang();						// do! and output current Vasp

	virtual Vasp *x_work() = 0;
};




#define VASP_SETUP(op) FLEXT_SETUP(vasp_##op);  



// base class for unary operations

class vasp_unop:
	public vasp_tx
{
	FLEXT_HEADER(vasp_unop,vasp_tx)

protected:
	vasp_unop(BL withto = false);

	virtual Vasp *x_work();
	virtual Vasp *tx_work();
};


// base class for binary operations

class vasp_binop:
	public vasp_tx
{
	FLEXT_HEADER(vasp_binop,vasp_tx)

protected:
	vasp_binop(I argc,t_atom *argv,BL withto = false);

	// assignment functions
	virtual V a_vasp(I argc,t_atom *argv);
	virtual V a_list(I argc,t_atom *argv); 
	virtual V a_float(F f); 
	virtual V a_complex(I argc,t_atom *argv); 
	virtual V a_vector(I argc,t_atom *argv); 

	virtual Vasp *x_work();
	virtual Vasp *tx_work(const Argument &arg);

	Argument arg;

private:
	FLEXT_CALLBACK_G(a_vasp)
	FLEXT_CALLBACK_G(a_list)
	FLEXT_CALLBACK_1(a_float,F)
	FLEXT_CALLBACK_G(a_complex)
	FLEXT_CALLBACK_G(a_vector)
};


// base class for non-parsed (list) arguments

class vasp_anyop:
	public vasp_tx
{
	FLEXT_HEADER(vasp_anyop,vasp_tx)

protected:
	vasp_anyop(I argc,t_atom *argv,BL withto = false);

	// assignment functions
	virtual V a_list(I argc,t_atom *argv); 
/*
	virtual V a_vasp(I argc,t_atom *argv);
	virtual V a_float(F f); 
	virtual V a_complex(I argc,t_atom *argv); 
	virtual V a_vector(I argc,t_atom *argv); 
*/
	virtual Vasp *x_work();
	virtual Vasp *tx_work(const Argument &arg);

	Argument arg;

private:
	FLEXT_CALLBACK_G(a_list)
/*
	FLEXT_CALLBACK_G(a_vasp)
	FLEXT_CALLBACK_1(a_float,F)
	FLEXT_CALLBACK_G(a_complex)
	FLEXT_CALLBACK_G(a_vector)
*/
};



#define VASP_UNARY(name,op,to)														\
class vasp__##op:																\
	public vasp_unop															\
{																				\
	FLEXT_HEADER(vasp__##op,vasp_unop)											\
public:																			\
	vasp__##op(): vasp_unop(to) {}															\
protected:																		\
	virtual Vasp *tx_work() { return VaspOp::m_##op(ref,&dst); }							\
};																				\
FLEXT_LIB(name,vasp__##op)

#define VASP_UNARY_SETUP(op) FLEXT_SETUP(vasp__##op);  


#define VASP_BINARY(name,op,to)													\
class vasp__ ## op:															\
	public vasp_binop															\
{																				\
	FLEXT_HEADER(vasp__##op,vasp_binop)										\
public:																			\
	vasp__##op(I argc,t_atom *argv): vasp_binop(argc,argv,to) {}					\
protected:																		\
	virtual Vasp *tx_work(const Argument &arg) { return VaspOp::m_##op(ref,arg,&dst); }		\
};																				\
FLEXT_LIB_G(name,vasp__##op)

#define VASP_BINARY_SETUP(op) FLEXT_SETUP(vasp__##op);  


#define VASP_ANYOP(name,op,to)													\
class vasp__ ## op:															\
	public vasp_anyop															\
{																				\
	FLEXT_HEADER(vasp__##op,vasp_anyop)										\
public:																			\
	vasp__##op(I argc,t_atom *argv): vasp_anyop(argc,argv,to) {}					\
protected:																		\
	virtual Vasp *tx_work(const Argument &arg) { return VaspOp::m_##op(ref,arg,&dst); }		\
};																				\
FLEXT_LIB_G(name,vasp__##op)

#define VASP_ANYOP_SETUP(op) FLEXT_SETUP(vasp__##op);  


#endif
