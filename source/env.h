/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_BPTS_H
#define __VASP_BPTS_H

#include "vasp.h"

class Bpts
{
public:
	Bpts(I argc,t_atom *argv);
//	Bpts(const Bpts &p);
	~Bpts();

	V Clear();

	BL Ok() const { return pos != NULL && val != NULL; }

	I Count() const { return cnt; }
	const R *Pos() const { return pos; }
	const R *Val() const { return val; }

protected:
	I cnt;
	R *pos,*val;
};

#endif

