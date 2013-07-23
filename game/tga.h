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

struct TGA_t {
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

class tga_tex : public texture {
	TGAHeader	tgaheader;
	TGA_t		tga;
	Texture		tex;
//	int glLoadTexture();
	bool LoadCompressedTGA(const char* filename, FILE* file);
	bool LoadUncompressedTGA(const char* filename, FILE* file);
	int glLoadTexture();
public :
	// constructor destructor
	tga_tex(const char* filename);
	~tga_tex();
	virtual unsigned int get_tex_id() const { return tex.texID; }
	virtual int get_width() const { return tex.width; }
	virtual int get_height() const { return tex.height;	}
	virtual textype get_tex_type() const { return TGA; }
	virtual void set_tex_id(unsigned int texid) { tex.texID = texid; }
	virtual unsigned char* get_data() const { return tex.imageData; }
	virtual int get_bpp() const { return tex.bpp; }
};

#endif // __3DSTEREO_TGA_HEADER_DEFINED__