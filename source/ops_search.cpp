/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "ops_search.h"
#include "util.h"
#include "oploop.h"


BL VecOp::d_search(OpParam &p) 
{ 
	const I off = p.srch.offs;
	const S val = p.srch.val;
	const R cur = p.rsdt[off];

	I i,ofl = -1,ofr = -1;
	
	if(p.srch.dir <= 0) {
		BL y = cur >= val,end = false;
		i = off-1;
		_D_WHILE(i >= 0 && !end) {
			BL y2 = p.rsdt[i] >= val;
			if(y != y2) {
				if(p.srch.slope <= 0 && y2) { end = true; break; }
				if(p.srch.slope >= 0 && !y2) { end = true; break; }
			}
			y = y2;
			--i;
		}

		if(i >= 0) ofl = i;
	}

	if(p.srch.dir >= 0) {
		BL y = cur >= val,end = false;
		i = off+1; 
		_D_WHILE(i < p.frames && !end) {
			BL y2 = p.rsdt[i] >= val;
			if(y != y2) {
				if(p.srch.slope <= 0 && !y2) { end = true; break; }
				if(p.srch.slope >= 0 && y2) { end = true; break; }
			}
			y = y2;
			++i;
		}

		if(i < p.frames) ofr = i;
	}

	if(!p.srch.dir) {
		if(ofl >= 0) {
			p.srch.dif = ofl-off;
			if(ofr >= 0 && abs(p.srch.dif) < abs(ofr-off)) p.srch.dif = ofr-off;
		}
		else 
			p.srch.dif = ofr >= 0?ofr-off:0;
	}
	else if(p.srch.dir > 0) 
		p.srch.dif = ofr >= 0?ofr-off:0;
	else
		p.srch.dif = ofl >= 0?ofl-off:0;

	return true;
}

Vasp *VaspOp::m_search(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst,BL st) 
{ 
	Vasp *ret = NULL;
	if(src.Vectors() > 1) 
		post("%s -  More than one vector in vasp!",p.opName());
	else if(arg.CanbeFloat() || (arg.IsList() && arg.GetList().Count() >= 1)) {
		I fr = src.ChkFrames();
		I o = src.Vector(0).Offset();
		I sz = src.Buffer(0)->Frames();

		Vasp all(src);
		if(st) {
			// search start point
			p.srch.offs = o;

			// set bounds of search buffer
			all.Offset(0);
			all.Frames(fr+o);  // all frames of buffer
		}
		else {
			// search end point
			p.srch.offs = o+fr;
			// check if current offset is past buffer
			if(p.srch.offs >= sz) p.srch.offs = sz-1;

			// set bounds of search buffer
			all.Offset(o);
			all.Frames(sz-o);  // all frames of buffer			
		}

		RVecBlock *vecs = GetRVecs(p.opname,all,dst);
		if(vecs) {
			p.srch.val = arg.IsList()?flext::GetAFloat(arg.GetList()[0]):arg.GetAFloat();

			ret = DoOp(vecs,VecOp::d_search,p);

			if(st) o += p.srch.dif,fr -= p.srch.dif;
			else fr += p.srch.dif;

			if(ret) {
				ret->Offset(o);
				ret->Frames(fr);
				ret->Frames(ret->ChkFrames());
			}

			delete vecs;
		}
	}
	else
		post("%s - no arguments: no operation",p.opName());

	return ret;
}


class vasp_search:
	public vasp_anyop
{																				
	FLEXT_HEADER(vasp_search,vasp_anyop)
public:			
	
	vasp_search(I argc,t_atom *argv): 
		vasp_anyop(argc,argv,VASP_ARG_R(0),false,XletCode(xlet::tp_float,0)),
		slope(0),dir(0)
	{
		FLEXT_ADDMETHOD_I(0,"dir",m_dir);
		FLEXT_ADDMETHOD_I(0,"slope",m_slope);
	}

	virtual Vasp *do_work(OpParam &p) = 0;
		
	virtual Vasp *tx_work(const Argument &arg) 
	{ 
		OpParam p(thisName(),1);													
		p.srch.dir = dir;
		p.srch.slope  = slope;

		Vasp *ret = do_work(p);
		if(ret) ToOutFloat(1,p.srch.dif);
		return ret;
	}

	V m_dir(I d) { dir = d; }
	V m_slope(I sl) { slope = sl; }

protected:
	I dir,slope;

private:
	FLEXT_CALLBACK_I(m_dir)
	FLEXT_CALLBACK_I(m_slope)
};																				


class vasp_soffset:
	public vasp_search
{																				
	FLEXT_HEADER(vasp_soffset,vasp_search)
public:			
	vasp_soffset(I argc,t_atom *argv): vasp_search(argc,argv) {}
	virtual Vasp *do_work(OpParam &p) { return VaspOp::m_soffset(p,ref,arg,&dst); }
};																				
FLEXT_LIB_V("vasp, vasp.offset= vasp.o=",vasp_soffset)


class vasp_sframes:
	public vasp_search
{																				
	FLEXT_HEADER(vasp_sframes,vasp_search)
public:			
	vasp_sframes(I argc,t_atom *argv): vasp_search(argc,argv) {}
	virtual Vasp *do_work(OpParam &p) { return VaspOp::m_sframes(p,ref,arg,&dst); }
};																				
FLEXT_LIB_V("vasp, vasp.frames= vasp.f=",vasp_sframes)


