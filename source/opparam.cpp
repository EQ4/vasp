#include "opparam.h"
//#include <math.h>

// Duplication of breakpoint lists should be avoided
OpParam::Arg &OpParam::Arg::operator =(const Arg &op)
{
	Clear();

	switch(argtp = op.argtp) {
	case arg_x:	x = op.x; break;
	case arg_v:	v = op.v; break;
	case arg_l:	{
		// Copy breakpoint list (find a different way...)
		l.pts = op.l.pts;
		l.r = new R[l.pts];
		l.i = new R[l.pts];
		for(I i = 0; i < l.pts; ++i) 
			l.r[i] = op.l.r[i],l.i[i] = op.l.i[i];
		break;
	}
	}

	return *this;
}

V OpParam::Arg::Clear()
{
	if(argtp == arg_l) {
		if(l.r) delete[] l.r;	
		if(l.i) delete[] l.i;	
	}
	argtp = arg_;
}

OpParam::Arg &OpParam::Arg::SetX(S r,S i)
{
	Clear();
	argtp = arg_x;
	x.r = r,x.i = i;
	return *this;
}

OpParam::Arg &OpParam::Arg::SetV(S *r,I rs,S *i,I is)
{
	Clear();
	argtp = arg_v;
	v.rdt = r,v.rs = rs;
	v.idt = i,v.is = is;
	return *this;
}

OpParam::Arg &OpParam::Arg::SetL(I pts,R *r,R *i)
{
	Clear();
	argtp = arg_l;
	l.pts = pts;
	l.r = new R[pts];
	l.i = new R[pts];
	for(I ix = 0; ix < pts; ix) 
		l.r[ix] = r[ix],l.i[ix] = i[ix];
	return *this;
}



/*
V OpParam::SDR_Rev() { SR_Rev(); DR_Rev(); }
V OpParam::SDI_Rev() { SI_Rev(); DI_Rev(); }
V OpParam::SDC_Rev() { SDR_Rev(); SDI_Rev(); }
V OpParam::ADR_Rev() { AR_Rev(); DR_Rev(); }
V OpParam::ADI_Rev() { AI_Rev(); DI_Rev(); }
V OpParam::ADC_Rev() { ADR_Rev(); ADI_Rev(); }
V OpParam::SADR_Rev() { SR_Rev(); AR_Rev(); DR_Rev(); }
V OpParam::SADI_Rev() { SI_Rev(); AI_Rev(); DI_Rev(); }
V OpParam::SADC_Rev() { SADR_Rev(); SADI_Rev(); }
*/

OpParam::OpParam(const C *opnm,I nargs): 
	opname(opnm),frames(0),args(0),arg(NULL),
	/*part(false),*/ ovrlap(false),revdir(false) 
{
	InitArgs(nargs);
}

OpParam::~OpParam() { Clear(); }

V OpParam::InitArgs(I n)
{
	if(arg) Clear();
	args = n;
	if(args) arg = new Arg[args];
}

V OpParam::Clear()
{
	if(arg) { delete[] arg; arg = NULL;	}
	args = 0;
}


/*! \brief Reverse direction of real vector operation 
	\todo Check for existence of vectors!
*/
V OpParam::R_Rev() 
{ 

	SR_Rev(); 
	DR_Rev();
	AR_Rev(); 
	revdir = true;
}

/*! \brief Reverse direction of complex vector operation 
	\todo Check for existence of vectors!
*/
V OpParam::C_Rev() 
{ 
	SR_Rev(); SI_Rev(); 
	DR_Rev(); DI_Rev();
	AR_Rev(); AI_Rev(); 
	revdir = true;
}


V OpParam::AR_Rev(I bl) 
{ 
	if(arg[bl].argtp == Arg::arg_v && arg[bl].v.rdt) 
		arg[bl].v.rdt -= (frames-1)*(arg[bl].v.rs = -arg[bl].v.rs); 
}

V OpParam::AI_Rev(I bl) 
{ 
	if(arg[bl].argtp == Arg::arg_v && arg[bl].v.idt) 
		arg[bl].v.idt -= (frames-1)*(arg[bl].v.is = -arg[bl].v.is); 
}

BL OpParam::AR_In(I bl) const { return arg[bl].argtp == Arg::arg_v && arg[bl].v.rdt && rddt > arg[bl].v.rdt && rddt < arg[bl].v.rdt+frames*arg[bl].v.rs; } 
BL OpParam::AI_In(I bl) const { return arg[bl].argtp == Arg::arg_v && arg[bl].v.idt && iddt > arg[bl].v.idt && iddt < arg[bl].v.idt+frames*arg[bl].v.is; } 

BL OpParam::AR_Can(I bl) const { return arg[bl].argtp != Arg::arg_v || !arg[bl].v.rdt || arg[bl].v.rdt <= rddt || arg[bl].v.rdt >= rddt+frames*rds; } 
BL OpParam::AI_Can(I bl) const { return arg[bl].argtp != Arg::arg_v || !arg[bl].v.idt || arg[bl].v.idt <= iddt || arg[bl].v.idt >= iddt+frames*ids; } 

BL OpParam::AR_Ovr(I bl) const { return arg[bl].argtp == Arg::arg_v && arg[bl].v.rdt && rddt != arg[bl].v.rdt && rddt < arg[bl].v.rdt+frames*arg[bl].v.rs && arg[bl].v.rdt < rddt+frames*rds; } 
BL OpParam::AI_Ovr(I bl) const { return arg[bl].argtp == Arg::arg_v && arg[bl].v.idt && iddt != arg[bl].v.idt && iddt < arg[bl].v.idt+frames*arg[bl].v.is && arg[bl].v.idt < iddt+frames*ids; } 



BL OpParam::AR_In() const
{
	for(I i = 0; i < args; ++i) 
		if(AR_In(i)) return true;
	return false;
}

BL OpParam::AI_In() const
{
	for(I i = 0; i < args; ++i) 
		if(!AI_In(i)) return true;
	return false;
}

BL OpParam::AR_Can() const
{
	for(I i = 0; i < args; ++i) 
		if(!AR_Can(i)) return false;
	return true;
}

BL OpParam::AI_Can() const
{
	for(I i = 0; i < args; ++i) 
		if(!AI_Can(i)) return false;
	return true;
}

BL OpParam::AR_Ovr() const
{
	for(I i = 0; i < args; ++i) 
		if(!AR_Ovr(i)) return false;
	return true;
}

BL OpParam::AI_Ovr() const
{
	for(I i = 0; i < args; ++i) 
		if(!AI_Ovr(i)) return false;
	return true;
}


V OpParam::AR_Rev()
{
	for(I i = 0; i < args; ++i) AR_Rev(i);
}

V OpParam::AI_Rev()
{
	for(I i = 0; i < args; ++i) AI_Rev(i);
}



