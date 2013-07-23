//////////////////////////////////////////////////////////////////////////
// Texture header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic DUBOUCHET
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// general texture loading factory

#ifndef __3DSTEREO_TEXTURE_HEADER_DEFINED__
#define __3DSTEREO_TEXTURE_HEADER_DEFINED__

// types of texture file formats
enum textype
{
	PCX,
	BMP,
	TGA
};

class texture {
public :
	virtual textype get_tex_type() const = 0;
	virtual int get_width() const = 0;
	virtual int get_height() const = 0;
	virtual unsigned int get_tex_id() const = 0;
	virtual void set_tex_id(unsigned int texid) = 0;
	virtual unsigned char* get_data() const = 0;
	virtual int get_bpp() const = 0;
};

texture* LoadTexture(const char *filename);

#endif // __3DSTEREO_TEXTURE_HEADER_DEFINED__