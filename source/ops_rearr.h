/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __VASP_REARR_FLT_H
#define __VASP_REARR_FLT_H

// Rearrange buffer 

namespace VecOp {
	BL d_shift(OpParam &p);
	BL d_rot(OpParam &p);
	BL d_mirr(OpParam &p);
}

namespace VaspOp {
	Vasp *m_shift(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL,BL sh = true,BL symm = false);  // shift buffer
	inline Vasp *m_xshift(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_shift(p,src,arg,dst,true,true); }  // shift buffer (symmetrically)
	inline Vasp *m_rot(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL) { return m_shift(p,src,arg,dst,false,false); } // rotate buffer
	inline Vasp *m_xrot(OpParam &p,Vasp &src,const Argument &arg,Vasp *dst = NULL)  { return m_shift(p,src,arg,dst,false,true); }  // rotate buffer (symmetrically)
	Vasp *m_mirr(OpParam &p,Vasp &src,Vasp *dst = NULL,BL symm = false);  //! mirror buffer
	inline Vasp *m_xmirr(OpParam &p,Vasp &src,Vasp *dst = NULL) { return m_mirr(p,src,dst,true); } //! mirror buffer (symmetrically)

}

#endif
