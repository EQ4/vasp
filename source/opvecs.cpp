/* 

VASP modular - vector assembling signal processor / objects for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "main.h"

// just transform real vectors
Vasp *Vasp::fr_nop(const C *op,F v,V (*f)(F *,F,I))
{
	BL ok = true;
	for(I ci = 0; ok && ci < this->Vectors(); ++ci) {
		VBuffer *bref = this->Buffer(ci);		
		if(!bref->Data()) {
			post("%s(%s) - dst vector (%s) is invalid",thisName(),op,bref->Name());
			ok = false; // really return?
		}
		else {
			f(bref->Data()+bref->Offset(),v,bref->Length());
			bref->Dirty();
		}
		delete bref;
	}

	// output Vasp
	return ok?new Vasp(*this):NULL;
}

// just transform complex vector pairs
Vasp *Vasp::fc_nop(const C *op,const CX &cx,V (*f)(F *,F *,F,F,I))
{
	BL ok = true;

	if(this->Vectors()%2 != 0) {
		post("%s(%s) - number of dst vectors is odd - omitting last vector",thisName(),op);
		// clear superfluous vector?
	}

	for(I ci = 0; ok && ci < this->Vectors()/2; ++ci) {
		VBuffer *bre = this->Buffer(ci*2),*bim = this->Buffer(ci*2+1); // complex channels

		if(!bre->Data()) {
			post("%s(%s) - real dst vector (%s) is invalid",thisName(),op,bre->Name());
			ok = false; // really return?
		}
		else if(!bim->Data()) {
			post("%s(%s) - imag dst vector (%s) is invalid",thisName(),op,bim->Name());
			ok = false; // really return?
		}
		else {
			I frms = bre->Length();
			if(frms != bim->Length()) {
				post("%s - real/imag dst vector length is not equal - using minimum",thisName());
				frms = min(frms,bim->Length());

				// clear the rest?
			}

			f(bre->Data()+bre->Offset(),bim->Data()+bre->Offset(),cx.real,cx.imag,frms);
			bre->Dirty(); bim->Dirty();
		}

		delete bre;
		delete bim;
	}

	// output corrected!! Vasp
	return ok?new Vasp(*this):NULL;
}

// just transform complex vector pairs
Vasp *Vasp::fc_nop(const C *op,const Argument &arg,const nop_funcs &f)
{
	if(arg.IsFloat()) 
		return fc_nop(op,CX(arg.GetFloat()),f);
	else if(arg.IsComplex()) 
		return fc_nop(op,arg.GetComplex(),f);
	else 
		return arg.IsNone()?new Vasp(*this):NULL;
}


// for real values or single real vector
Vasp *Vasp::fr_arg(const C *op,F v,V (*f)(F *,F,I))
{
	BL ok = true;
	for(I ci = 0; ok && ci < this->Vectors(); ++ci) {
		VBuffer *bref = this->Buffer(ci);		
		if(!bref->Data()) {
			post("%s(%s) - dst vector (%s) is invalid",thisName(),op,bref->Name());
			ok = false; // really return?
		}
		else {
			f(bref->Data()+bref->Offset(),v,bref->Length());
			bref->Dirty();
		}
		delete bref;
	}

	// output Vasp
	return ok?new Vasp(*this):NULL;
}

Vasp *Vasp::fr_arg(const C *op,const Vasp &arg,V (*f)(F *,const F *,I))
{
	if(!arg.Ok()) {
		post("%s(%s) - invalid argument vasp detected and ignored",thisName(),op);
		return NULL;
	}

	if(arg.Vectors() > 1) {
		post("%s(%s) - using only first src vector",thisName(),op);
	}

	BL ok = true;

	VBuffer *barg = arg.Buffer(0);
	if(!barg->Data()) {
		post("%s(%s) - src vector (%s) is invalid",thisName(),op,barg->Name());
		ok = false; // really return?
	}
	else {
		I frms = barg->Frames();

		for(I ci = 0; ok && ci < this->Vectors(); ++ci) {
			VBuffer *bref = this->Buffer(ci);		

			if(!bref->Data()) {
				post("%s(%s) - dst vector (%s) is invalid",thisName(),op,bref->Name());
				ok = false; // really break?
			}
			else {
				I frms1 = frms;
				if(frms1 != bref->Length()) {
					post("%s(%s) - src/dst vector length not equal - using minimum",thisName(),op);
					frms1 = min(frms1,bref->Length());

					// clear rest?
				}

				f(bref->Data()+bref->Offset(),barg->Data()+barg->Offset(),frms1);
				bref->Dirty();
			}

			delete bref;
		}
	}

	delete barg;

	// output corrected Vasp
	return ok?new Vasp(*this):NULL;
}

// for real values or single real vector
Vasp *Vasp::fr_arg(const C *op,const Argument &arg,const arg_funcs &f)
{
	if(arg.IsFloat()) 
		return fr_arg(op,arg.GetFloat(),f); // real input
	else if(arg.IsVasp()) 
		return fr_arg(op,arg.GetVasp(),f); // single vector source
	else
		return NULL;
}


// for complex values or complex vector pair
Vasp *Vasp::fc_arg(const C *op,const CX &cx,V (*f)(F *,F *,F,F,I))
{
	// complex (or real) input

	if(this->Vectors()%2 != 0) {
		post("%s(%s) - number of src vectors is odd - omitting last vector",thisName(),op);
		// clear superfluous vector?
	}

	BL ok = true;

	for(I ci = 0; ci < this->Vectors()/2; ++ci) {
		VBuffer *bre = this->Buffer(ci*2),*bim = this->Buffer(ci*2+1); // complex channels

		if(!bre->Data()) {
			post("%s(%s) - real dst vector (%s) is invalid",thisName(),op,bre->Name());
			ok = false;
		}
		if(!bim->Data()) {
			post("%s(%s) - imag dst vector (%s) is invalid",thisName(),op,bim->Name());
			ok = false;
		}
		else {
			I frms = bre->Length();
			if(frms != bim->Length()) {
				post("%s(%s) - real/imag dst vector length is not equal - using minimum",thisName(),op);
				frms = min(frms,bim->Length());

				// clear the rest?
			}

			f(bre->Data()+bre->Offset(),bim->Data()+bim->Offset(),cx.real,cx.imag,frms);
			bre->Dirty(); bim->Dirty();
		}

		delete bre;
		delete bim;
	}

	// output corrected vasp
	return ok?new Vasp(*this):NULL;
}

// for complex values or complex vector pair
Vasp *Vasp::fc_arg(const C *op,const Vasp &arg,V (*f)(F *,F *,const F *,const F *,I))
{
	if(!arg.Ok()) {
		post("%s(%s) - invalid argument vasp detected and ignored",thisName(),op);
		return NULL;
	}

	BL ok = true;
	VBuffer *brarg = arg.Buffer(0),*biarg = arg.Buffer(1);

	if(!biarg) {
		post("%s(%s) - only one src vector - setting imaginary part to 0",thisName(),op);					
	}
	else if(arg.Vectors() > 2) {
		post("%s(%s) - using only first two src vectors",thisName(),op);
	}

	if(!brarg->Data()) {
		post("%s(%s) - real src vector (%s) is invalid",thisName(),op,brarg->Name());
		ok = false;
	}
	else if(biarg && !biarg->Data()) {
		post("%s(%s) - imag src vector (%s) is invalid",thisName(),op,biarg->Name());
		ok = false;
	}
	else {
		I frms = brarg->Length();
		if(biarg && frms != biarg->Length()) {
			post("%s(%s) - real/imag src vector length is not equal - using minimum",thisName(),op);
			frms = min(frms,biarg->Length());
		}

		if(this->Vectors()%2 != 0) {
			post("%s(%s) - number of dst vectors is odd - omitting last vector",thisName(),op);
			// clear superfluous vector?
		}

		for(I ci = 0; ok && ci < this->Vectors()/2; ++ci) {
			VBuffer *brref = this->Buffer(ci*2),*biref = this->Buffer(ci*2+1);		

			if(!brref->Data()) {
				post("%s(%s) - real dst vector (%s) is invalid",thisName(),op,brref->Name());
				ok = false; // really break?
			}
			else if(biref && !biref->Data()) {
				post("%s(%s) - imag dst vector (%s) is invalid",thisName(),op,biref->Name());
				ok = false; // really break?
			}
			else {
				I frms1 = frms;
				if(frms1 != brref->Length() || frms1 != biref->Length()) {
					post("%s(%s) - source/target vector length not equal - using minimum",thisName(),op);
					frms1 = min(frms1,min(biref->Length(),brref->Length()));

					// clear rest?
				}

				f(brref->Data()+brref->Offset(),biref->Data()+biref->Offset(),brarg->Data()+brarg->Offset(),biarg->Data()+biarg->Offset(),frms1);
				brref->Dirty(); biref->Dirty();
			}

			delete brref;
			delete biref;
		}
	}

	delete brarg;
	if(biarg) delete biarg;

	// output corrected!! Vasp
	return ok?new Vasp(*this):NULL;
}

Vasp *Vasp::fc_arg(const C *op,const Argument &arg,const arg_funcs &f)
{
	if(arg.IsFloat()) 
		return fc_arg(op,CX(arg.GetFloat()),f);
	else if(arg.IsComplex()) 
		return fc_arg(op,arg.GetComplex(),f);
	else if(arg.IsVasp()) 
		return fc_arg(op,arg.GetVasp(),f); // pairs of vectors
	else
		return NULL;
}

// for multiple vectors
Vasp *Vasp::fm_arg(const C *op,const Vasp &arg,V (*f)(F *,const F *,I))
{
	if(!arg.Ok()) {
		post("%s(%s) - invalid src Vasp",thisName(),op);
		return NULL;
	}

	if(!this->Ok()) {
		post("%s(%s) - invalid dst Vasp",thisName(),op);
		return NULL;
	}

	I vecs = this->Vectors();
	if(vecs != arg.Vectors()) {
		post("%s(%s) - unequal src/dst vector count - using minimum",thisName(),op);

		vecs = min(vecs,arg.Vectors());
		// clear the rest?
	}

	BL ok = true,warned = false;

	// do
	for(I ci = 0; ok && ci < vecs; ++ci) {
		VBuffer *bref = this->Buffer(ci),*barg = arg.Buffer(ci);

		if(!bref->Data()) {
			post("%s(%s) - dst vector (%s) is invalid",thisName(),op,bref->Name());
			ok = false; // really return?
		}
		if(!barg->Data()) {
			post("%s(%s) - src vector (%s) is invalid",thisName(),op,barg->Name());
			ok = false; // really return?
		}
		else {
			I frms = bref->Length();
			if(frms != barg->Length()) {
				if(!warned) { // warn only once (or for each vector?) 
					post("%s(%s) - src/dst vector length not equal - using minimum",thisName(),op);
					warned = true;
				}

				frms = min(frms,barg->Length());
				// clear the rest?
			}

			f(bref->Data()+bref->Offset(),barg->Data()+barg->Offset(),frms);
			bref->Dirty();
		}

		delete bref;
		delete barg;
	}

	// output corrected Vasp
	return ok?new Vasp(*this):NULL;
}

// for multiple vectors

Vasp *Vasp::fm_arg(const C *op,const Argument &arg,const arg_funcs &f)
{
	return arg.IsVasp()?fm_arg(op,arg.GetVasp(),f):NULL;
}
