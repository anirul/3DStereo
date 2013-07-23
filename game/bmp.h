//////////////////////////////////////////////////////////////////////////
// BMP header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic DUBOUCHET
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// inspired from OpenGL Game programming

#ifndef __3DSTEREO_BMP_HEADER_DEFINED__
#define __3DSTEREO_BMP_HEADER_DEFINED__

#ifndef WIN32
typedef struct tagBITMAPINFOHEADER{
	unsigned long		biSize;
	long				biWidth;
	long				biHeight;
	unsigned short		biPlanes;
	unsigned short		biBitCount;
	unsigned long		biCompression;
	unsigned long		biSizeImage;
	long				biXPelsPerMeter;
	long				biYPelsPerMeter;
	unsigned long		biClrUsed;
	unsigned long		biClrImportant;
} BITMAPINFOHEADER;
#endif

class bmp_tex : public texture {
	BITMAPINFOHEADER bitmapInfoHeader;
	// bitmap image data
	unsigned char *bitmapImage;
	unsigned int glTexID;
	void LoadBitmapFile(const char *filename);
public :
	bmp_tex(const char *filename);
	~bmp_tex();
	virtual unsigned int get_tex_id() const { return glTexID; }
	virtual int get_width() const { return bitmapInfoHeader.biWidth; }
	virtual int get_height() const { return bitmapInfoHeader.biHeight;	}
	virtual textype get_tex_type() const { return BMP; }
	virtual void set_tex_id(unsigned int texid) { glTexID = texid; }
	virtual unsigned char* get_data() const { return bitmapImage; }
	virtual int get_bpp() const { return 32; }
};

#endif // __3DSTEREO_BMP_HEADER_DEFINED__