/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "ops_rearr.h"
#include "oploop.h"
#include "oppermute.h"

/*! \brief vasp shift or rotation
	\todo units for shift
*/
Vasp *VaspOp::m_shift(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst,BL shift,BL symm) 
{
	Vasp *ret = NULL;
	RVecBlock *vecs = GetRVecs(p.opname,src,dst);
	if(vecs) {
		if(arg.IsList() && arg.GetList().Count() >= 1 && flext::CanbeFloat(arg.GetList()[0])) {
			// shift length
			p.sh.sh = flext::GetAFloat(arg.GetList()[0]);
		}
		else {
			post("%s - invalid argument -> set to 0",p.opname);
			p.sh.sh = 0;
		}

		ret = DoOp(vecs,shift?VecOp::d_shift:VecOp::d_rot,p,symm);
		delete vecs;
	}

	return ret;
}


/*! \brief shift buffer
*/
BL VecOp::d_shift(OpParam &p) 
{ 
	if(p.ovrlap) {
		post("%s - cannot operate on overlapped vectors",p.opname);
		return false;
	}

	I ish = (I)p.sh.sh;
	if(p.sh.sh != ish) { // integer shift
		// requires interpolation
		post("non-integer shift not implemented - truncating to integer");
		p.sh.sh = ish;
	}

	p.SkipOddMiddle();
	
	if(p.symm == 1) ish = -ish;

	I aish = abs(ish);
	if(aish > p.frames) aish = ish = p.frames;

	I i,cnt = p.frames-aish;
	const S *sd = p.rsdt-ish*p.rss;
	S *dd = p.rddt;

	if(ish > 0) {
		sd += (p.frames-1)*p.rss,dd += (p.frames-1)*p.rds;
		p.rss = -p.rss,p.rds = -p.rds;
	}

	// do shift
	if(cnt > 0) {
		if(p.rss == 1 && p.rds == 1) 
			_D_LOOP(i,cnt) *(dd++) = *(sd++); _E_LOOP
		else if(p.rss == -1 && p.rds == -1) 
			_D_LOOP(i,cnt) *(dd--) = *(sd--); _E_LOOP
		else 
			_D_LOOP(i,cnt) *dd = *sd,sd += p.rss,dd += p.rds; _E_LOOP
	}

	// fill spaces
	if(p.sh.fill) {
		S vfill = p.sh.fill == 1?0:dd[-p.rds];
		I aish = abs(ish);
		if(p.rds == 1) 
			_D_LOOP(i,aish) *(dd++) = vfill; _E_LOOP
		else if(p.rds == -1) 
			_D_LOOP(i,aish) *(dd--) = vfill; _E_LOOP
		else 
			_D_LOOP(i,aish) *dd = vfill,dd += p.rds; _E_LOOP
	}

	return true;
}


class vasp_shift:
	public vasp_anyop
{																				
	FLEXT_HEADER(vasp_shift,vasp_anyop)
public:			
	
	vasp_shift(I argc,const t_atom *argv): 
		vasp_anyop(argc,argv,VASP_ARG_I(0),true),
		fill(xsf_zero)
	{
		FLEXT_ADDATTR_VAR1_E("fill",fill);
	}

	enum xs_fill {
		xsf__ = -1,  // don't change
		xsf_none = 0,xsf_zero,xsf_edge
	};	

	virtual Vasp *do_shift(OpParam &p) { return VaspOp::m_shift(p,ref,arg,&dst); }
		
	virtual Vasp *tx_work(const Argument &arg) 
	{ 
		OpParam p(thisName(),0);													
		p.sh.fill  = (I)fill;

		Vasp *ret = do_shift(p);
		return ret;
	}

	virtual V m_help() { post("%s - Shifts buffer data",thisName()); }

protected:
	xs_fill fill;

private:
	FLEXT_ATTRVAR_E(fill,xs_fill)
};																				
FLEXT_LIB_V("vasp, vasp.shift",vasp_shift)


class vasp_xshift:
	public vasp_shift
{																				
	FLEXT_HEADER(vasp_xshift,vasp_shift)
public:			
	
	vasp_xshift(I argc,const t_atom *argv): vasp_shift(argc,argv) {}

	virtual Vasp *do_shift(OpParam &p) { return VaspOp::m_xshift(p,ref,arg,&dst); }
		
	virtual V m_help() { post("%s - Shifts buffer data symmetrically (in two halves)",thisName()); }
};																				
FLEXT_LIB_V("vasp, vasp.xshift",vasp_xshift)


inline int rotation(int ij, int n,OpParam &p) { return (ij+n-p.sh.ish)%n; }

#define ROTBLOCK 1024

/*! \brief rotate buffer
	\todo implement temporary storage for faster transformation (use abstract permute algorithm)
*/
BL VecOp::d_rot(OpParam &p) 
{ 
	if(p.ovrlap) {
		post("%s - cannot operate on overlapped vectors",p.opname);
		return false;
	}

	p.sh.ish = (I)p.sh.sh;
	if(p.sh.sh != p.sh.ish) { 
		// requires interpolation
		post("%s - non-integer shift not implemented - truncating to integer",p.opname);
	}

	p.SkipOddMiddle();
	
	p.sh.ish = p.sh.ish%p.frames;
	if(p.symm == 1) p.sh.ish = -p.sh.ish;

/*
	if(p.frames >= ROTBLOCK) {
		//use temporary space;
		S *tmp = new S[ROTBLOCK];
		
		delete[] tmp;
	}
	else 
*/
		PERMUTATION(S,1,p,rotation);
	return true; 
}

VASP_ANYOP("vasp.rot",rot,0,true,VASP_ARG_I(0),"Rotates buffer data")
VASP_ANYOP("vasp.xrot",xrot,0,true,VASP_ARG_I(0),"Rotates buffer data symmetrically (in two halves)")


/*! \brief mirror buffer
*/
BL VecOp::d_mirr(OpParam &p) 
{ 
	if(p.ovrlap) {
		post("%s - cannot operate on overlapped vectors",p.opname);
		return false;
	}

	p.SkipOddMiddle();
	
	if(p.rsdt == p.rddt) {
		S *dl = p.rddt,*du = p.rddt+(p.frames-1)*p.rds;
		_D_WHILE(dl < du)
			register S t;
			t = *dl; *dl = *du; *du = t;
			dl += p.rds,du -= p.rds;
		_E_WHILE
	}
	else {
		I i;
		const S *ds = p.rsdt;
		S *dd = p.rddt+(p.frames-1)*p.rds;
		_D_LOOP(i,p.frames) 
		*dd = *ds,ds += p.rss,dd -= p.rds;
		_E_LOOP
	}
	return true; 
}

/*! \brief vasp mirror
*/
Vasp *VaspOp::m_mirr(OpParam &p,Vasp &src,Vasp *dst,BL symm) 
{
	Vasp *ret = NULL;
	RVecBlock *vecs = GetRVecs(p.opname,src,dst);
	if(vecs) {
		ret = DoOp(vecs,VecOp::d_mirr,p,symm);
		delete vecs;
	}
	return ret;
}

VASP_UNARY("vasp.mirr",mirr,true,"Mirrors buffer data")
VASP_UNARY("vasp.xmirr",xmirr,true,"Mirrors buffer data symmetrically (in two halves)")


