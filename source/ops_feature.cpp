/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "main.h"
#include <math.h>


// --- integrate/differentiate

/*! \brief Integration
	\remark The delay of the result is +/- one sample, depending on the direction of the calculation 
	
	\todo different modes how to initialize first carry?
	\todo repetition count
*/
BL VecOp::d_int(OpParam &p) 
{ 
	if(!p.revdir)
		post("%s - reversed operation direction due to overlap: opposite sample delay",p.opname);

	register S d = p.intdif.carry;
	for(; p.frames-- ; p.rsdt += p.rss,p.rddt += p.rds) { 
		register S d1 = *p.rsdt; 
		*p.rddt = d1+d,d = d1; 
	}
	p.intdif.carry = d;
	return true; 
}

/*! \brief Differentiation
	\remark The delay of the result is +/- one sample, depending on the direction of the calculation 

	\todo different modes how to initialize first carry?
	\todo repetition count
*/
BL VecOp::d_dif(OpParam &p) 
{ 
	if(!p.revdir)
		post("%s - reversed operation direction due to overlap: opposite sample delay",p.opname);

	register S d = p.intdif.carry;
	for(; p.frames-- ; p.rsdt += p.rss,p.rddt += p.rds) { 
		register S d1 = *p.rsdt; 
		*p.rddt = d1-d,d = d1; 
	}
	p.intdif.carry = d;
	return true; 
}

/*! \brief Does vasp integration/differentiation.

	\param arg argument list 
	\param dst destination vasp (NULL for in-place operation)
	\param inv true for differentiation
	\return normalized destination vasp
*/
Vasp *VaspOp::m_int(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst,BL inv) 
{ 
	Vasp *ret = NULL;
	RVecBlock *vecs = GetRVecs(p.opname,src,dst);
	if(vecs) {
		p.intdif.carry = 0,p.intdif.rep = 1;
		if(arg.IsList() && arg.GetList().Count() >= 1) p.intdif.rep = flx::GetAInt(arg.GetList()[0]);
		
		if(p.intdif.rep < 0) {
			post("%s - invalid repetition count (%i) -> set to 1",p.opname,p.intdif.rep);
			p.intdif.rep = 1;
		}
		
		ret = DoOp(vecs,inv?VecOp::d_dif:VecOp::d_int,p);
		delete vecs;
	}
	return ret;
}


// --- find peaks

BL higher(S a,S b) { return a > b; }
BL lower(S a,S b) { return a < b; }

/*! \brief Find peaks or valleys (depending on cmp function)
	\param rep repetition count
	
	\remark real peak search is mangled into complex domain

	\todo how to treat <=, >=
	\todo separate real and complex functionality
*/

static BL d_vlpk(OpParam &p,BL cmpf(S a,S b)) 
{ 
	I dpeaks = p.frames*p.peaks.density;
	if(dpeaks < 1) dpeaks = 1;

	I cnt;
	do {
		cnt = 0;

		I i;
		S *rdst = p.rddt,*rsrc = p.rsdt;
		S *idst = p.iddt,*isrc = p.isdt;
		
		if(!p.peaks.cx || !idst) idst = rdst,p.ids = p.rds;
		if(!p.peaks.cx || !isrc) isrc = rsrc,p.iss = p.rss;

		// preset sample values
		S d1 = -1,d0 = -1,dn = -1;

		// search first non-null sample
		for(i = 0; i < p.frames; ++i) 
			if((dn = sqabs(rsrc[i*p.rss],isrc[i*p.iss])) != 0)
				break; // non-null -> break!
			else 
				rdst[i*p.rds] = idst[i*p.ids] = 0; // copy null samples to dst

		// i points to first non-null sample

		while(i < p.frames) {

			// current samples -> previous samples
			d1 = d0,d0 = dn;

			// save current index 
			I ci = i;

			// search next non-null sample
			dn = -1;
			while(++i < p.frames) 
				if((dn = sqabs(rsrc[i*p.rss],isrc[i*p.iss])) != 0) {
					break; // non-null -> break!
				}
				else 
					rdst[i*p.rds] = idst[i*p.ids] = 0;

			if((d1 < 0 || cmpf(d0,d1)) && (dn < 0 || cmpf(d0,dn))) {
				// is peak/valley
				rdst[ci*p.rds] = rsrc[ci*p.rss];
				idst[ci*p.ids] = isrc[ci*p.iss];
				++cnt;
			}
			else
				rdst[ci*p.rds] = idst[ci*p.ids] = 0;
		}				
	} while(cnt > dpeaks);

	p.peaks.density = p.frames?(R)cnt/p.frames:(cnt?1:0);
	return true; 
}

inline BL d_peaks(OpParam &p) { return d_vlpk(p,higher); }
inline BL d_valleys(OpParam &p) { return d_vlpk(p,lower); }
inline BL d_rpeaks(OpParam &p) { return d_vlpk(p,higher); }
inline BL d_rvalleys(OpParam &p) { return d_vlpk(p,lower); }

/*! \brief Finds peaks or valleys in a vasp.

	\param arg argument list 
	\param arg.rep repetition count
	\param dst destination vasp (NULL for in-place operation)
	\param inv true for valley operation
	\return normalized destination vasp
*/
Vasp *VaspOp::m_peaks(OpParam &p,Vasp &src,Vasp *dst,BL inv) 
{ 
	Vasp *ret = NULL;
	RVecBlock *vecs = GetRVecs(p.opname,src,dst);
	if(vecs) {
		p.peaks.cx = false;
		ret = DoOp(vecs,inv?d_valleys:d_peaks,p);
		delete vecs;
	}
	return ret;
}



/*! \brief Finds peaks or valleys by radius in a complex vasp.

	\param arg argument list 
	\param arg.rep repetition count
	\param dst destination vasp (NULL for in-place operation)
	\param inv true for valley operation
	\return normalized destination vasp
*/
Vasp *VaspOp::m_rpeaks(OpParam &p,Vasp &src,Vasp *dst,BL inv) 
{ 
	Vasp *ret = NULL;
	CVecBlock *vecs = GetCVecs(p.opname,src,dst);
	if(vecs) {
		p.peaks.cx = true;
		ret = DoOp(vecs,inv?d_rvalleys:d_rpeaks,p);
		delete vecs;
	}
	return ret;
}


class vasp_peaks:
	public vasp_anyop
{																				
	FLEXT_HEADER(vasp_peaks,vasp_anyop)
public:			
	
	// \todo should have float outlet to connect to number box!!
	vasp_peaks(I argc,t_atom *argv): vasp_anyop(argc,argv,true,1) {}

	virtual Vasp *do_peaks(OpParam &p) { return VaspOp::m_peaks(p,ref,&dst); }
		
	virtual Vasp *tx_work(const Argument &arg) 
	{ 
		OpParam p(thisName(),0);													
		
		if(arg.IsList() && arg.GetList().Count() >= 1 && CanbeFloat(arg.GetList()[0])) {
			p.peaks.density = GetAFloat(arg.GetList()[0]);
		}
		else {
			if(!arg.IsNone()) post("%s - invalid desity argument -> set to 1",p.opname);
			p.peaks.density = 1;
		}
		
		Vasp *ret = do_peaks(p);
		ToOutFloat(1,p.peaks.density);
		return ret;
	}
};																				
FLEXT_LIB_G("vasp.peaks",vasp_peaks)


class vasp_valleys:
	public vasp_peaks
{																				
	FLEXT_HEADER(vasp_valleys,vasp_peaks)
public:			
	vasp_valleys(I argc,t_atom *argv): vasp_peaks(argc,argv) {}
	virtual Vasp *do_peaks(OpParam &p) { return VaspOp::m_valleys(p,ref,&dst); }
};																				
FLEXT_LIB_G("vasp.valleys",vasp_valleys)


class vasp_rpeaks:
	public vasp_peaks
{																				
	FLEXT_HEADER(vasp_rpeaks,vasp_peaks)
public:			
	vasp_rpeaks(I argc,t_atom *argv): vasp_peaks(argc,argv) {}
	virtual Vasp *do_peaks(OpParam &p) { return VaspOp::m_rpeaks(p,ref,&dst); }
};																				
FLEXT_LIB_G("vasp.rpeaks",vasp_rpeaks)


class vasp_rvalleys:
	public vasp_peaks
{																				
	FLEXT_HEADER(vasp_rvalleys,vasp_peaks)
public:			
	vasp_rvalleys(I argc,t_atom *argv): vasp_peaks(argc,argv) {}
	virtual Vasp *do_peaks(OpParam &p) { return VaspOp::m_rvalleys(p,ref,&dst); }
};																				
FLEXT_LIB_G("vasp.rvalleys",vasp_rvalleys)

