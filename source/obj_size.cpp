/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "classes.h"
#include "util.h"

/*! \class vasp_size
	\remark \b vasp.size
	\brief Resize buffer.
	\since 0.0.6
	\param cmdln.1 [_time=0] - size of buffer
	\param inlet.1 vasp - is stored and output triggered
	\param inlet.1 bang - triggers output
	\param inlet.1 set - vasp to be stored 
    \param inlet.2 _time - offset into buffer(s)
	\retval outlet vasp - modified vasp

	\attention Normally vasp vectors have individual offsets - this operations sets all the buffer sizes to equal values.
	\todo Implement unit processing.
*/
class vasp_size:
	public vasp_tx
{
	FLEXT_HEADER(vasp_size,vasp_tx)

public:
	vasp_size(I argc,t_atom *argv):
		size(0),sets(false)
	{
		if(argc >= 1 && CanbeFloat(argv[0]))
			m_size(GetAFloat(argv[0]));
		else if(argc)
			post("%s - Offset argument invalid -> ignored",thisName());

		AddInAnything();
		AddInFloat();
		AddOutAnything();
		SetupInOut();

		FLEXT_ADDMETHOD(1,m_size);
	}

	V m_size(F s) 
	{ 
		size = s; 
		sets = true;
	}

	virtual Vasp *x_work() 
	{ 
		Vasp *ret = new Vasp(ref); 
		if(sets) ret->Size(size);
		return ret;
	}

	virtual V m_help() { post("%s - Set the size of the vector buffers",thisName()); }
protected:
	I size;
	BL sets;

private:
	FLEXT_CALLBACK_F(m_size);
};

FLEXT_LIB_G("vasp.size",vasp_size)




/*! \class vasp_dsize
	\remark \b vasp.size+
	\brief Sets vector buffer sizes differentially.
	\since 0.0.6
	\param cmdln.1 [_time=0] - increase offset of into buffer(s)
	\param inlet.1 vasp - is stored and output triggered
	\param inlet.1 bang - triggers output
	\param inlet.1 set - vasp to be stored 
    \param inlet.2 _time - increase of offset into buffer(s)
	\retval outlet vasp - modified vasp

	\todo Implement unit processing
*/
class vasp_dsize:
	public vasp_size
{
	FLEXT_HEADER(vasp_dsize,vasp_size)

public:
	vasp_dsize(I argc,t_atom *argv): vasp_size(argc,argv) {}

	virtual Vasp *x_work() 
	{ 
		Vasp *ret = new Vasp(ref); 
		if(sets) ret->SizeD(size);
		return ret;
	}

	virtual V m_help() { post("%s - Increase the size of the vector buffers",thisName()); }
};

FLEXT_LIB_G("vasp.size+",vasp_dsize)



/*! \class vasp_qsize
	\remark \b vasp.size?
	\brief Get size of a vector buffer.
	\since 0.0.6
	\param inlet vasp - is stored and output triggered
	\param inlet bang - triggers output
	\param inlet set - vasp to be stored 
	\retval outlet _time - offset into vector buffer

	\note Outputs 0 if vasp is undefined or invalid
	\note Only works for a vasp with one vector. No output otherwise.

	\todo Implement unit processing
	\todo Should we provide a cmdln default vasp?
	\todo Should we inhibit output for invalid vasps?
*/
class vasp_qsize:
	public vasp_op
{
	FLEXT_HEADER(vasp_qsize,vasp_op)

public:

	vasp_qsize()
	{
		AddInAnything();
//		AddOutAnything();
		AddOutFloat();
		SetupInOut();
	}

	virtual V m_bang() 
	{ 
		if(!ref.Ok())
			post("%s - Invalid vasp!",thisName());
		else if(ref.Vectors() > 1) 
			post("%s - More than one vector in vasp!",thisName());
		else {
			I s = 0;
			if(ref.Vectors() == 1) {
				VBuffer *buf = ref.Buffer(0);
				if(buf) {
					s = buf->Frames();
					delete buf;
				}
			}
			//! \todo unit processing
//			ToOutVasp(0,ref);
			ToOutFloat(0,s);
		}
	}

	virtual V m_help() { post("%s - Get the buffer size of a vector",thisName()); }
};

FLEXT_LIB("vasp.size?",vasp_qsize)

