VASP modular - vector assembling signal processor
Object library for Max/MSP and PD

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

Donations for further development of the package are highly appreciated.

----------------------------------------------------------------------------

DOWNLOAD:
=========

http://www.parasitaere-kapazitaeten.net/vasp

----------------------------------------------------------------------------

Package files:
- readme.txt: this one
- gpl.txt,license.txt,mixfft.txt: license stuff
- changes.txt,todo.txt: additional info
- pd-help/*: VASP help
- pd/*: VASP abstractions
- config-*,build-*,makefile.*,*.cw: Build
- source/*: VASP sources

----------------------------------------------------------------------------

GOALS/FEATURES:
===============

VASP is a package for PD or MaxMSP consisting of a number of externals extending 
these systems with functions for non-realtime array-based audio data processing. 
VASP is capable of working in the background, therefore not influencing eventual 
dsp signal processing.

----------------------------------------------------------------------------

USAGE:
======

IMPORTANT INFORMATION for all PD users:
---------------------------------------

For VASP and its documentation to work properly, you have to specify a 
PD command line like

Linux/OSX: pd -path /usr/local/lib/pd/extra/vasp/pd -lib vasp/vasp
Windows:  pd -path c:\audio\pd\extra\vasp\pd -lib vasp\vasp


IMPORTANT INFORMATION for all MaxMSP users:
-------------------------------------------

It is advisable to put the vasp object library file into the "max-startup" folder. 
Hence it will be loaded at Max startup.

If you want alternatively to load the vasp library on demand, 
create a "vasp" object somewhere. The library is then loaded.

----------------------------------------------------------------------------

COMPILATION:
============

You will need the flext C++ layer for PD and Max/MSP externals.
see http://www.parasitaere-kapazitaeten.net/ext/flext


The package should at least compile (and is tested) with the following compilers:

- PD @ Windows:
o Microsoft Visual C++ 6: use vasp.dsp or edit "config-pd-msvc.txt" and run "sh build-pd-msvc.sh"

- PD @ linux:
o GCC: edit "config-pd-linux.txt" and run "sh build-pd-linux.sh"

- PD @ MacOSX:
o GCC: edit "config-pd-darwin.txt" and run "sh build-pd-darwin.sh"

- Max/MSP @ MacOS9:
o Metrowerks CodeWarrior V6: edit vasp.cw project and build


