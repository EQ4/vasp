/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_VBUFFER_H
#define __VASP_VBUFFER_H

#include "main.h"

class VSymbol
{
public:
	VSymbol(t_symbol *s = NULL): sym(s) { Inc(); }
	VSymbol(const VSymbol &s): sym(s.sym) { Inc(); }
	~VSymbol() { Dec(); }
	
	BL Ok() const { return sym != NULL; }
	V Clear() { Dec(); sym = NULL; }

//	V *Thing() { return sym?flext_base::GetThing(sym):NULL; }
//	V Thing(V *th) { if(sym) flext_base::GetThing(sym); }
	
	VSymbol &operator =(const VSymbol &s) { Dec(); sym = s.sym; Inc(); return *this; }
	
	t_symbol *Symbol() { return sym; }
	const t_symbol *Symbol() const { return sym; }
	const C *Name() const { return flext_base::GetAString(Symbol()); }
	
protected:
	V Inc();
	V Dec();

	t_symbol *sym;
};

class VBuffer
{
public:
	virtual ~VBuffer() {}

	virtual BL Ok() const = 0;
	virtual I Frames() const = 0;
	virtual V Frames(I fr,BL keep) = 0;

	virtual I Channels() const = 0;
	virtual S *Data() = 0;

	virtual V Refresh() {}
	virtual V Dirty() {}

	S *Pointer() { return Data()+Offset()*Channels()+Channel(); }

	virtual VSymbol Symbol() const = 0;
	const C *Name() const { return Symbol().Name(); }

	I Channel() const { return chn; }
	V Channel(I c) { chn = c; }

	I Offset() const { return offs; }
	V Offset(I o) { offs = o; }

	I Length() const { return len; }
	V Length(I l) { len = l; }

protected:
	VBuffer(I c = 0,I l = 0,I o = 0): chn(c),offs(o),len(l) {}

	I chn,offs,len;
};


class SysBuf:
	public VBuffer
{
public:
	SysBuf(const VSymbol &s,I chn = 0,I len = -1,I offs = 0) { Set(s,chn,len,offs); }

	virtual BL Ok() const { return buf.Ok(); }
	virtual V Refresh() { buf.Dirty(true); }
	virtual V Dirty() { buf.Dirty(false); }

	virtual VSymbol Symbol() const { return buf.Symbol(); }

	SysBuf &Set(const VSymbol &s,I chn = 0,I len = -1,I offs = 0);

	virtual I Frames() const { return buf.Frames(); }
	virtual V Frames(I fr,BL keep) { buf.Frames(fr,keep); }

	virtual I Channels() const { return buf.Channels(); }
	virtual S *Data() { return buf.Data(); }

protected:
	flext_base::buffer buf;
};


class BufEntry;

class ImmBuf:
	public VBuffer
{
public:
	ImmBuf(I len);
	ImmBuf(BufEntry *e,I len = -1,I offs = 0);

	virtual BL Ok() const { return entry != NULL; }

	virtual VSymbol Symbol() const;

	virtual I Frames() const;
	virtual V Frames(I fr,BL keep);

	virtual I Channels() const { return 1; }
	virtual S *Data();

protected:
	BufEntry *entry;
};


#endif
