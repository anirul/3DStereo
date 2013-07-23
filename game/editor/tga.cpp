#include "tga.h"

// Load a TGA file
tga_tex::tga_tex(char * filename)
{
	FILE * fTGA;
	fTGA = fopen(filename, "rb");
	memset(&texture, 0, sizeof(Texture));
	memset(&tga, 0, sizeof(TGA));
	texture.texID = -1;

	if(fTGA == NULL) {
		printf("WARNING : Could not open texture file : %s\n", filename);
	} else if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0) {
		printf("WARNING : Could not read file header : %s\n", filename);
		if(fTGA != NULL) {
			fclose(fTGA);
		}
	} else if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	{
		if (LoadUncompressedTGA(filename, fTGA))
			glLoadTexture();
		else
			texture.texID = -1;
	} else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0) {
		if (LoadCompressedTGA(filename, fTGA))
			glLoadTexture();
		else
			texture.texID = -1;
	} else {
		printf("WARNING : TGA file be type 2 or type 10 : %s\n", filename);
		fclose(fTGA);
	}
}

tga_tex::~tga_tex() {
//	if (texture.imageData)
//		free(texture.imageData);
}

bool tga_tex::LoadUncompressedTGA(char * filename, FILE * fTGA){

	#ifdef _DEBUG
		printf("INFO : Loading uncompressed TGA : %s\n", filename);
	#endif
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}	

	texture.width  = tga.header[1] * 256 + tga.header[0];
	texture.height = tga.header[3] * 256 + tga.header[2];
	texture.bpp	= tga.header[4];
	tga.Width = texture.width;
	tga.Height = texture.height;
	tga.Bpp	= texture.bpp;	

	if ((texture.width <= 0) || (texture.height <= 0) || ((texture.bpp != 24) && (texture.bpp !=32))) {
		printf("WARNING : Invalid texture information : %s\n", filename);
		if (fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	if (texture.bpp == 24)
		texture.type = GL_RGB;
	else
		texture.type = GL_RGBA;

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
	texture.imageData = (GLubyte *)malloc(tga.imageSize);

	if(texture.imageData == NULL) {
		printf("WARNING : Could not allocate memory for image : %s\n", filename);
		fclose(fTGA);
		return false;
	}

	if (fread(texture.imageData, 1, tga.imageSize, fTGA) != tga.imageSize) {
		printf("WARNING : Could not read image data : %s\n", filename);
		if (texture.imageData != NULL) {
			free(texture.imageData);
			texture.imageData = NULL;
		}
		fclose(fTGA);
		return false;
	}

	// Byte Swapping Optimized By Steve Thomas
	for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel) {
		texture.imageData[cswap] ^= texture.imageData[cswap+2] ^= 
			texture.imageData[cswap] ^= texture.imageData[cswap+2];
	}

	fclose(fTGA);
	return true;
}

bool tga_tex::LoadCompressedTGA(char * filename, FILE * fTGA) {

	#ifdef _DEBUG
		printf("INFO : Loading compressed TGA : %s\n", filename);
	#endif
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		printf("WARNING : Could not read info header : %s\n", filename);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	texture.width  = tga.header[1] * 256 + tga.header[0];
	texture.height = tga.header[3] * 256 + tga.header[2];
	texture.bpp	= tga.header[4];
	tga.Width = texture.width;
	tga.Height = texture.height;
	tga.Bpp = texture.bpp;

	if((texture.width <= 0) || (texture.height <= 0) || ((texture.bpp != 24) && (texture.bpp !=32))) {
		printf("WARNING : Invalid texture information : %s\n", filename);
		if(fTGA != NULL)
			fclose(fTGA);
		return false;
	}

	if(texture.bpp == 24)
		texture.type = GL_RGB;
	else
		texture.type = GL_RGBA;

	tga.bytesPerPixel	= (tga.Bpp / 8);
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);
	texture.imageData	= (GLubyte *)malloc(tga.imageSize);

	if(texture.imageData == NULL) {
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
			if (texture.imageData != NULL) {
				free(texture.imageData);
				texture.imageData = NULL;
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
					if(texture.imageData != NULL) {
						free(texture.imageData);
						texture.imageData = NULL;
					}
					return false;
				}
				// write to memory
				texture.imageData[currentbyte		] = colorbuffer[2];
				texture.imageData[currentbyte + 1	] = colorbuffer[1];
				texture.imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4) {
					texture.imageData[currentbyte + 3] = colorbuffer[3];
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
					if(texture.imageData != NULL) {
						free(texture.imageData);
						texture.imageData = NULL;
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
				if(texture.imageData != NULL) {
					free(texture.imageData);
					texture.imageData = NULL;
				}
				return false;
			}

			for(short counter = 0; counter < chunkheader; counter++) {
				texture.imageData[currentbyte		] = colorbuffer[2];
				texture.imageData[currentbyte + 1	] = colorbuffer[1];
				texture.imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4) {
					texture.imageData[currentbyte + 3] = colorbuffer[3];
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
					if(texture.imageData != NULL) {
						free(texture.imageData);
						texture.imageData = NULL;
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
	if (!texture.imageData) {
		printf("ERROR : no texture pointer.\n");
		return -1;
	}
	if (texture.texID != -1) {
		printf("WARING : file was already loaded.\n");
		return texture.texID;
	}
	// Typical Texture Generation Using Data From The TGA ( CHANGE )
	glGenTextures(1, &texture.texID);
	glBindTexture(GL_TEXTURE_2D, texture.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, 
		texture.bpp / 8, 
		texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	return texture.texID;
}