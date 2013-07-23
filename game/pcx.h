//////////////////////////////////////////////////////////////////////////
// PCX header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic DUBOUCHET
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// inspired from OpenGL Game programming

#ifndef __3DSTEREO_PCX_HEADER_DEFINED__
#define __3DSTEREO_PCX_HEADER_DEFINED__

// only partial pcx file header
typedef struct
{
	unsigned char manufacturer;
	unsigned char version;
	unsigned char encoding;
	unsigned char bits;
	unsigned char xMin;
	unsigned char yMin;
	unsigned char xMax;
	unsigned char yMax;
	unsigned char *palette;
} PCXHEADER;

class pcx_tex : public texture {
	PCXHEADER pcxHeader;
	int width;
	int height;
	unsigned int scaledHeight;
	unsigned int scaledWidth;
	int glTexID;
	unsigned char *data;		// the texture data
	unsigned char *palette;
	unsigned char* LoadPCXFile(const char* filename);
	void LoadPCXTexture(const char* filename);
public :
	pcx_tex(const char* filename);
	~pcx_tex();
	virtual unsigned int get_tex_id() const { return glTexID; }
	virtual int get_width() const { return width; }
	virtual int get_height() const { return height;	}
	virtual textype get_tex_type() const { return PCX; }
	virtual void set_tex_id(unsigned int texid) { glTexID = texid; }
	virtual unsigned char* get_data() const { return data; }
	virtual int get_bpp() const { return 24; }
};

#endif // __3DSTEREO_PCX_HEADER_DEFINED__