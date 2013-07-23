//////////////////////////////////////////////////////////////////////////
// Main Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// main header for the scene editor test program.

#ifndef __3DSTEREO_MAIN_HEADER_DEFINED__
#define __3DSTEREO_MAIN_HEADER_DEFINED__

// lib graphic
#ifdef WIN32
	#include <windows.h>
#endif
#ifdef WIN32
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#else
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#endif

// misc C
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// misc C++
#include <iostream>
#include <string>
#include <vector>
#include <map>

// local lib
#include "amc.h"

// #define STEREO_3D 1

int main(int ac, char** av);

#endif // __3DSTEREO_MAIN_HEADER_DEFINED__
