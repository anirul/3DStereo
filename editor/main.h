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
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

// lib3ds
#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

// misc C
#include <string.h>
#include <config.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// misc C++
#include <iostream>
#include <string>
#include <vector>
#include <map>

// local libs
#include "tga.h"

int main(int ac, char** av);

#endif // __3DSTEREO_MAIN_HEADER_DEFINED__