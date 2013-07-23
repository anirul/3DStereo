#include "main.h"

texture* LoadTexture(const char* filename) {
	char *extStr;

	// get extension from filename
	extStr = strchr(filename, '.');
	extStr++;

	// set the texture type based on extension of filename
	if ((strcmp(extStr, "BMP") == 0) || (strcmp(extStr, "bmp") == 0)) {
		bmp_tex* p = new bmp_tex(filename);
		return dynamic_cast<texture*>(p);
	} else if ((strcmp(extStr, "PCX") == 0) || (strcmp(extStr, "pcx") == 0)) {
		pcx_tex* p = new pcx_tex(filename);
		return dynamic_cast<texture*>(p);
	} else if ((strcmp(extStr, "TGA") == 0) || (strcmp(extStr, "tga") == 0)) {
		tga_tex* p = new tga_tex(filename);
		return dynamic_cast<texture*>(p);
	}
	printf("Cannot load texture : %s, %s unknown", filename, extStr);
	return NULL;
}