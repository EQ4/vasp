/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "classes.h"
#include "util.h"


namespace flext {

/*! \class vasp_part
	\remark \b vasp.part
	\brief Gets parts of vasp vectors.
	\since 0.0.1
	\param cmdln.1 list - list of part lengts
	\param inlet.1 vasp - is stored and output triggered
	\param inlet.1 bang - triggers output
	\param inlet.1 set - vasp to be stored 
    \param inlet.2 list - list of part lengts
 	\retval outlet.1 vasp - consecutive vasp parts
 	\retval outlet.2 vasp - remainder

	\todo Implement unit processing.
	\remarks Output zero length vasps?
*/
class vasp_part:
	public vasp_op
{
	FLEXT_HEADER(vasp_part,vasp_op)

public:
	vasp_part(I argc,t_atom *argv):
		part(NULL),parts(0)
	{
		m_part(argc,argv);

		AddInAnything(2);
		AddOutAnything(2);
		SetupInOut();

		FLEXT_ADDMETHOD_(1,"list",m_part);
	}

	~vasp_part() { if(part) delete[] part; }

	V m_part(I argc,t_atom *argv) 
	{ 
		if(part) delete[] part; parts = 0;
		part = new I[argc]; 
		for(I i = 0; i < argc; ++i) {
			BL warn = false;
			I p = (I)GetAFloat(argv[i]); // \todo unit processing
			if(p < 0 && !warn) {
				post("%s - invalid part length(s) -> set to 0",thisName());
				p = 0; warn = true;
			}
			part[i] = p; ++parts;
		}
	}

	virtual V m_bang() 
	{ 
		if(!ref.Ok()) {
			post("%s - Invalid vasp!",thisName());
			return;
		}

		I fr = ref.Frames(),o = 0,f = 0;
		for(I i = 0; i < parts && (fr < 0 || fr); ++i) {
			I p = part[i];
			if(fr >= 0) { p = min(p,fr); fr -= p; }

			Vasp ret(ref); 
			ret.Frames(p);
			ret.OffsetD(o);
			ToOutVasp(0,ret);

			o += p;
		}

		if(fr) {
			Vasp ret(ref); 
			ret.Frames(fr);
			ret.OffsetD(o);
			ToOutVasp(1,ret);
		}
	}

	virtual V m_help() { post("%s - Return consecutive vasps with lengths given by argument list",thisName()); }
protected:
	I parts,*part;

	FLEXT_CALLBACK_V(m_part)
};

FLEXT_LIB_V("vasp, vasp.part",vasp_part)

}



