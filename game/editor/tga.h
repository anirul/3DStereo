//////////////////////////////////////////////////////////////////////////
// TGA header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic DUBOUCHET
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// inspired from Nehe tutorial 33

#ifndef __3DSTEREO_TGA_HEADER_DEFINED__
#define __3DSTEREO_TGA_HEADER_DEFINED__

#include "main.h"

struct TGAHeader {
	GLubyte Header[12];
};

struct TGA {
	TGAHeader	tgaheader;
	GLubyte		header[6];
	GLuint		bytesPerPixel;
	GLuint		imageSize;
	GLuint		temp;
	GLuint		type;	
	GLuint		Height;
	GLuint		Width;
	GLuint		Bpp;
};

struct Texture
{
	GLubyte	* imageData;
	GLuint	bpp;
	GLuint	width;
	GLuint	height;
	GLuint	texID;
	GLuint	type;
};	

// Uncompressed TGA Header
const GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};
// Compressed TGA Header
const GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

class tga_tex {
	TGAHeader	tgaheader;
	TGA			tga;
	Texture		texture;
	int glLoadTexture();
	bool LoadCompressedTGA(char* filename, FILE* file);
	bool LoadUncompressedTGA(char* filename, FILE* file);
public :
	// constructor destructor
	tga_tex(char* filename);
	~tga_tex();
	int GetglID() const { return texture.texID; }
};

#endif // __3DSTEREO_TGA_HEADER_DEFINED__