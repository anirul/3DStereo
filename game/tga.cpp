#include "tga.h"

// Load a TGA file
tga_tex::tga_tex(const char * filename)
{
	FILE * fTGA;
	fTGA = fopen(filename, "rb");
	memset(&tex, 0, sizeof(Texture));
	memset(&tga, 0, sizeof(TGA_t));
	tex.texID = -1;

	if(fTGA == NULL)
		printf("WARNING : Could not open tex file : %s\n", filename);
	else if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
		printf("WARNING : Could not read file header : %s\n", filename);
	else if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		LoadUncompressedTGA(filename, fTGA);
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		LoadCompressedTGA(filename, fTGA);
	else
		printf("WARNING : TGA file be type 2 or type 10 : %s\n", filename);
	if (fTGA)
		fclose(fTGA);
	glLoadTexture();
}

tga_tex::~tga_tex() {
	if (tex.imageData)
		free(tex.imageData);
}

bool tga_tex::LoadUncompressedTGA(const char * filename, FILE * fTGA){

	#ifdef _DEBUG
		printf("INFO : Loading uncompressed TGA : %s\n", filename);
	#endif
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}	

	tex.width  = tga.header[1] * 256 + tga.header[0];
	tex.height = tga.header[3] * 256 + tga.header[2];
	tex.bpp	= tga.header[4];
	tga.Width = tex.width;
	tga.Height = tex.height;
	tga.Bpp	= tex.bpp;	

	if ((tex.width <= 0) || (tex.height <= 0) || ((tex.bpp != 24) && (tex.bpp !=32))) {
		printf("WARNING : Invalid tex information : %s\n", filename);
		if (fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	if (tex.bpp == 24)
		tex.type = GL_RGB;
	else
		tex.type = GL_RGBA;

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
	tex.imageData = (GLubyte *)malloc(tga.imageSize);

	if(tex.imageData == NULL) {
		printf("WARNING : Could not allocate memory for image : %s\n", filename);
		fclose(fTGA);
		return false;
	}

	if (fread(tex.imageData, 1, tga.imageSize, fTGA) != tga.imageSize) {
		printf("WARNING : Could not read image data : %s\n", filename);
		if (tex.imageData != NULL) {
			free(tex.imageData);
			tex.imageData = NULL;
		}
		fclose(fTGA);
		return false;
	}

	// Byte Swapping Optimized By Steve Thomas
	for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel) {
		tex.imageData[cswap] ^= tex.imageData[cswap+2] ^= 
			tex.imageData[cswap] ^= tex.imageData[cswap+2];
	}

	fclose(fTGA);
	return true;
}

bool tga_tex::LoadCompressedTGA(const char * filename, FILE * fTGA) {

	#ifdef _DEBUG
		printf("INFO : Loading compressed TGA : %s\n", filename);
	#endif
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		printf("WARNING : Could not read info header : %s\n", filename);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	tex.width  = tga.header[1] * 256 + tga.header[0];
	tex.height = tga.header[3] * 256 + tga.header[2];
	tex.bpp	= tga.header[4];
	tga.Width = tex.width;
	tga.Height = tex.height;
	tga.Bpp = tex.bpp;

	if((tex.width <= 0) || (tex.height <= 0) || ((tex.bpp != 24) && (tex.bpp !=32))) {
		printf("WARNING : Invalid tex information : %s\n", filename);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	if(tex.bpp == 24)
		tex.type = GL_RGB;
	else
		tex.type = GL_RGBA;

	tga.bytesPerPixel	= (tga.Bpp / 8);
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);
	tex.imageData	= (GLubyte *)malloc(tga.imageSize);

	if(tex.imageData == NULL) {
		printf("WARNING : Could not allocate memory for image : %s\n", filename);
		fclose(fTGA);
		return false;
	}

	GLuint pixelcount	= tga.Height * tga.Width;
	GLuint currentpixel	= 0;
	GLuint currentbyte	= 0;
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);

	do
	{
		GLubyte chunkheader = 0;

		if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) {
			printf("WARNING : Could not read RLE header : %s\n", filename);
			if (fTGA != NULL)
				fclose(fTGA);
			if (tex.imageData != NULL) {
				free(tex.imageData);
				tex.imageData = NULL;
			}
			return false;
		}

		if (chunkheader < 128) {
			chunkheader++;
			for (short counter = 0; counter < chunkheader; counter++) {
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {
					printf("WARNING : Could not read image data : %s\n", filename);
					if(fTGA != NULL)
						fclose(fTGA);
					if(colorbuffer != NULL) {
						free(colorbuffer);
						colorbuffer = NULL;
					}
					if(tex.imageData != NULL) {
						free(tex.imageData);
						tex.imageData = NULL;
					}
					return false;
				}
				// write to memory
				tex.imageData[currentbyte		] = colorbuffer[2];
				tex.imageData[currentbyte + 1	] = colorbuffer[1];
				tex.imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4) {
					tex.imageData[currentbyte + 3] = colorbuffer[3];
				}

				currentbyte += tga.bytesPerPixel;
				currentpixel++;

				if(currentpixel > pixelcount) {
					printf("WARNING : Too many pixels read : %s\n", filename);
					if (fTGA != NULL) 
						fclose(fTGA);
					if (colorbuffer != NULL) {
						free(colorbuffer);
						colorbuffer = NULL;
					}
					if(tex.imageData != NULL) {
						free(tex.imageData);
						tex.imageData = NULL;
					}
					return false;
				}
			}
		} else {
			chunkheader -= 127;
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)	{
				printf("WARNING : Could not read from file : %s\n", filename);
				if(fTGA != NULL) 
					fclose(fTGA);
				if(colorbuffer != NULL) {
					free(colorbuffer);
					colorbuffer = NULL;
				}
				if(tex.imageData != NULL) {
					free(tex.imageData);
					tex.imageData = NULL;
				}
				return false;
			}

			for(short counter = 0; counter < chunkheader; counter++) {
				tex.imageData[currentbyte		] = colorbuffer[2];
				tex.imageData[currentbyte + 1	] = colorbuffer[1];
				tex.imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4) {
					tex.imageData[currentbyte + 3] = colorbuffer[3];
				}

				currentbyte += tga.bytesPerPixel;
				currentpixel++;

				if(currentpixel > pixelcount) {
					printf("WARNING : Too many pixels read : %s\n", filename);

					if(fTGA != NULL)
						fclose(fTGA);
					if(colorbuffer != NULL) {
						free(colorbuffer);
						colorbuffer = NULL;
					}
					if(tex.imageData != NULL) {
						free(tex.imageData);
						tex.imageData = NULL;
					}
					return false;
				}
			}
		}
	} while(currentpixel < pixelcount);

	fclose(fTGA);
	return true;
}


int tga_tex::glLoadTexture() {
	if (!tex.imageData) {
		printf("ERROR : no tex pointer.\n");
		return -1;
	}
	if (tex.texID != -1) {
		printf("WARING : file was already loaded.\n");
		return tex.texID;
	}
	// Typical Texture Generation Using Data From The TGA ( CHANGE )
	glGenTextures(1, &tex.texID);
	glBindTexture(GL_TEXTURE_2D, tex.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, 
		tex.bpp / 8, 
		tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	return tex.texID;	
}