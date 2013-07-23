//////////////////////////////////////////////////////////////////////////
// Main Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// main header for the game test program.

#ifndef __3DSTEREO_MAIN_HEADER_DEFINED__
#define __3DSTEREO_MAIN_HEADER_DEFINED__

// stupid const
#define DTOR 0.0174532925
#define RTOD 57.2957795
#define WSCALE 30.0f

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
#include <fstream>
#include <string>
#include <vector>
#include <map>

// general libs
#include "new.h"
#include "math.h"
#include "timer.h"

// media libs
#include "texture.h"
#include "tga.h"
#include "bmp.h"
#include "pcx.h"
#include "camera.h"
#include "md2.h"
#include "m3ds.h"
#include "block.h"
#include "level.h"
#include "player.h"

// human interface libs
#include "font.h"
#include "winkeyb.h"
#include "glutwin.h"

// game class
#include "gamewin.h"

// main entry point
int main(int ac, char** av);

#endif // __3DSTEREO_MAIN_HEADER_DEFINED__