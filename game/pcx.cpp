#include "main.h"

pcx_tex::pcx_tex(const char* filename) {
	data = NULL;
	palette = NULL;
	LoadPCXTexture(filename);
	glTexID = -1;
}

pcx_tex::~pcx_tex() {
	if (data)
		free(data);
	if (palette)
		free(palette);
}

// LoadPCXFile()
// desc: loads a PCX file into memory
unsigned char* pcx_tex::LoadPCXFile(const char *filename)
{
	int idx = 0;					// counter index
	int c;							// used to retrieve a char from the file
	int i;							// counter index
	int numRepeat;      
	FILE *filePtr;					// file handle
	int width;						// pcx width
	int height;						// pcx height
	unsigned char *pixelData;		// pcx image data
	unsigned char *paletteData;		// pcx palette data

	// open PCX file
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// retrieve first character; should be equal to 10
	c = getc(filePtr);
	if (c != 10)
	{
		fclose(filePtr);
		return NULL;
	}

	// retrieve next character; should be equal to 5
	c = getc(filePtr);
	if (c != 5)
	{
		fclose(filePtr);
		return NULL;
	}

	// reposition file pointer to beginning of file
	rewind(filePtr);

	// read 4 characters of data to skip
	fgetc(filePtr);
	fgetc(filePtr);
	fgetc(filePtr);
	fgetc(filePtr);

	// retrieve leftmost x value of PCX
	pcxHeader.xMin = fgetc(filePtr);       // loword
	pcxHeader.xMin |= fgetc(filePtr) << 8; // hiword

	// retrieve bottom-most y value of PCX
	pcxHeader.yMin = fgetc(filePtr);       // loword
	pcxHeader.yMin |= fgetc(filePtr) << 8; // hiword

	// retrieve rightmost x value of PCX
	pcxHeader.xMax = fgetc(filePtr);       // loword
	pcxHeader.xMax |= fgetc(filePtr) << 8; // hiword

	// retrieve topmost y value of PCX
	pcxHeader.yMax = fgetc(filePtr);       // loword
	pcxHeader.yMax |= fgetc(filePtr) << 8; // hiword

	// calculate the width and height of the PCX
	width = pcxHeader.xMax - pcxHeader.xMin + 1;
	height = pcxHeader.yMax - pcxHeader.yMin + 1;

	// allocate memory for PCX image data
	pixelData = (unsigned char*)malloc(width*height);

	// set file pointer to 128th byte of file, where the PCX image data starts
	fseek(filePtr, 128, SEEK_SET);

	// decode the pixel data and store
	while (idx < (width*height))
	{
		c = getc(filePtr);
		if (c > 0xbf)
		{
			numRepeat = 0x3f & c;
			c = getc(filePtr);

			for (i = 0; i < numRepeat; i++)
			{
				pixelData[idx++] = c;
			}
		}
		else
			pixelData[idx++] = c;

		fflush(stdout);
	}

	// allocate memory for the PCX image palette
	paletteData = (unsigned char*)malloc(768);

	// palette is the last 769 bytes of the PCX file
	fseek(filePtr, -769, SEEK_END);

	// verify palette; first character should be 12
	c = getc(filePtr);
	if (c != 12)
	{
		fclose(filePtr);
		return NULL;
	}

	// read and store all of palette
	for (i = 0; i < 768; i++)
	{
		c = getc(filePtr);
		paletteData[i] = c;
	}

	// close file and store palette in header
	fclose(filePtr);
	pcxHeader.palette = paletteData;

	// return the pixel image data
	return pixelData;
}

// LoadPCXTexture()
// desc: loads a PCX image file as a texture
void pcx_tex::LoadPCXTexture(const char *filename)
{
	unsigned char *unscaledData;		// used to calculate pcx
	int i;								// index counter
	int j;								// index counter
	int twidth;							// width of texture
	int theight;						// height of texture

	// allocate memory for texture struct

	// load the PCX file into the texture struct
	data = LoadPCXFile(filename);
	if (data == NULL)
	{
		free(data);
		return;
	}

	// store the texture information
	palette = pcxHeader.palette;
	width = pcxHeader.xMax - pcxHeader.xMin + 1;
	height = pcxHeader.yMax - pcxHeader.yMin + 1;

	// allocate memory for the unscaled data
	unscaledData = (unsigned char*)malloc(width*height*4);

	// store the unscaled data via the palette
	for (j = 0; j < height; j++) 
	{
		for (i = 0; i < width; i++) 
		{
			unscaledData[4*(j*width+i)+0] = (unsigned char)palette[3*data[j*width+i]+0];
			unscaledData[4*(j*width+i)+1] = (unsigned char)palette[3*data[j*width+i]+1];
			unscaledData[4*(j*width+i)+2] = (unsigned char)palette[3*data[j*width+i]+2];
			unscaledData[4*(j*width+i)+3] = (unsigned char)255;
		}
	}

	// find width and height's nearest greater power of 2
	twidth = width;
	theight = height;

	// find width's
	i = 0;
	while (twidth)
	{
		twidth /= 2;
		i++;
	}
	scaledHeight = (long)pow(2, i-1);

	// find height's
	i = 0;
	while (theight)
	{
		theight /= 2;
		i++;
	}
	scaledWidth = (long)pow(2, i-1);

	// clear the texture data
	if (data != NULL)
	{
		free(data);
		data = NULL;
	}

	// reallocate memory for the texture data
	data = (unsigned char*)malloc(scaledWidth*scaledHeight*4);

	// use the GL utility library to scale the texture to the unscaled dimensions
	gluScaleImage (GL_RGBA, width, height, GL_UNSIGNED_BYTE, 
		unscaledData, scaledWidth, scaledHeight, GL_UNSIGNED_BYTE, data);

}