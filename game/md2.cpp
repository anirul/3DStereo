#include "main.h"
#include "md2.h"

// CalculateNormal()
// desc: given 3 points, calculates the normal to the points
void CalculateNormal( float *p1, float *p2, float *p3 )
{
	float a[3], b[3], result[3];
	float length;

	a[0] = p1[0] - p2[0];
	a[1] = p1[1] - p2[1];
	a[2] = p1[2] - p2[2];

	b[0] = p1[0] - p3[0];
	b[1] = p1[1] - p3[1];
	b[2] = p1[2] - p3[2];

	result[0] = a[1] * b[2] - b[1] * a[2];
	result[1] = b[0] * a[2] - a[0] * b[2];
	result[2] = a[0] * b[1] - b[0] * a[1];

	// calculate the length of the normal
	length = (float)sqrt(result[0]*result[0] + result[1]*result[1] + result[2]*result[2]);

	// normalize and specify the normal
	glNormal3f(result[0]/length, result[1]/length, result[2]/length);
}

// CMD2Model constructor
CMD2Model::CMD2Model()
{
	numVertices = 0;    // vertices
	numTriangles = 0;   // triangles
	numFrames = 0;      // frames
	numST = 0;          // texture coordinates
	frameSize = 0;      // needed?
	currentFrame = 0;   // current keyframe 
	nextFrame = 1;      // next keyframe
	interpol = 0.0;     // interpolation percent
	triIndex = NULL;    // triangle indices
	st = NULL;          // texture coordinate indices
	vertexList = NULL;  // vertices
	modelTex = NULL;    // skin/texture
	modelState = MODEL_IDLE;

	binome[MODEL_IDLE] = begend(0, 39);
	binome[MODEL_RUN] = begend(40, 46);
	binome[MODEL_SHOT_STAND] = begend(47, 60);
	binome[MODEL_SHOT_SHOULDER] = begend(61, 66);
	binome[MODEL_JUMP] = begend(67, 73);
	binome[MODEL_IDLE2] = begend(74, 95);
	binome[MODEL_SHOT_FALL] = begend(96, 112);
	binome[MODEL_IDLE3] = begend(113, 122);
	binome[MODEL_IDLE4] = begend(123, 135);
	binome[MODEL_CROUCH] = begend(136, 154);
	binome[MODEL_CROUCH_CRAWL] = begend(155, 161);
	binome[MODEL_CROUCH_WEAPON] = begend(162, 169);
	binome[MODEL_DYING_KNEE] = begend(170, 177);
	binome[MODEL_DYING_BACK] = begend(178, 185);
	binome[MODEL_DYING_FORWARD] = begend(186, 190);
	binome[MODEL_DYING_SLOW] = begend(191, 198);
}

// CMD2Model destructor
CMD2Model::~CMD2Model()
{
}

// CMD2Model::SetupSkin()
// access: private
// desc: sets up the model skin/texture for OpenGL
void CMD2Model::SetupSkin(texture *thisTexture)
{
	unsigned int texid;
	// set the proper parameters for an MD2 texture
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	thisTexture->set_tex_id(texid);

	switch (thisTexture->get_tex_type())
	{
	case BMP:
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 
			thisTexture->get_width(), thisTexture->get_height(), 
			GL_RGB, GL_UNSIGNED_BYTE, thisTexture->get_data());
		break;
	case PCX:
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 
			thisTexture->get_width(), thisTexture->get_height(),
			GL_RGBA, GL_UNSIGNED_BYTE, thisTexture->get_data());
		break;
	case TGA:
		gluBuild2DMipmaps(GL_TEXTURE_2D, 
			(thisTexture->get_bpp() == 32) ? GL_RGBA : GL_RGB,
			thisTexture->get_width(), thisTexture->get_height(),
			(thisTexture->get_bpp() == 32) ? GL_RGBA : GL_RGB, 
			GL_UNSIGNED_BYTE, thisTexture->get_data());
		break;
	default:
		break;
	}
}

// CMD2Model::Load()
// access: public
// desc: loads model and skin
int CMD2Model::Load(const char* modelFile, const char* skinFile)
{
	FILE *filePtr;                          // file pointer
	int fileLen;                            // length of model file
	char *buffer;                           // file buffer

	modelHeader_t *modelHeader;             // model header

	stIndex_t *stPtr;                       // texture data
	frame_t *frame;                              // frame data
	vector_t *vertexListPtr;                // index variable
	mesh_t *bufIndexPtr;          // index variables
	int i, j;                               // index variables

	// open the model file
	filePtr = fopen(modelFile, "rb");
	if (filePtr == NULL)
		return FALSE;

	// find length of file
	fseek(filePtr, 0, SEEK_END);
	fileLen = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	// read entire file into buffer
	buffer = new char [fileLen+1];
	fread(buffer, sizeof(char), fileLen, filePtr);

	// extract model file header from buffer
	modelHeader = (modelHeader_t*)buffer;

	vertexList = new vector_t [modelHeader->numXYZ * modelHeader->numFrames];

	numVertices = modelHeader->numXYZ;
	numFrames = modelHeader->numFrames;
	frameSize = modelHeader->framesize;

	for (j = 0; j < numFrames; j++)
	{
		frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize * j];

		vertexListPtr = (vector_t*)&vertexList[numVertices * j];
		for (i = 0; i < numVertices; i++)
		{
			vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
			vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
			vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
		}
	}

	modelTex = LoadTexture(skinFile);
	if (modelTex != NULL)
		SetupSkin(modelTex);
	else
		return FALSE;

	numST = modelHeader->numST;
	st = new texCoord_t [numST];

	stPtr = (stIndex_t*)&buffer[modelHeader->offsetST];
	for (i = 0; i < numST; i++)
	{
		st[i].s = (float)stPtr[i].s / (float)modelTex->get_width();
		st[i].t = (float)stPtr[i].t / (float)modelTex->get_height();
	}

	numTriangles = modelHeader->numTris;
	triIndex = new mesh_t [numTriangles];

	// point to triangle indexes in buffer
	bufIndexPtr = (mesh_t*)&buffer[modelHeader->offsetTris];

	// create a mesh (triangle) list
	for (j = 0; j < numFrames; j++)         
	{
		// for all triangles in each frame
		for(i = 0; i < numTriangles; i++)
		{
			triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
			triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
			triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
			triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
			triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
			triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
		}
	}

	// close file and free memory
	fclose(filePtr);
	delete [] buffer;

	currentFrame = 0;
	nextFrame = 1;
	interpol = 0.0;

	return TRUE;
}

// CMD2Model::LoadModel()
// access: public
// desc: loads model from file
int CMD2Model::LoadModel(char *modelFile)
{
	FILE *filePtr;                          // file pointer
	int fileLen;                            // length of model file
	char *buffer;                           // file buffer

	modelHeader_t *modelHeader;             // model header

	stIndex_t *stPtr;                       // texture data
	frame_t *frame;                              // frame data
	vector_t *vertexListPtr;                // index variable
	mesh_t *triIndex, *bufIndexPtr;         // index variables
	int i, j;                               // index variables

	// open the model file
	filePtr = fopen(modelFile, "rb");
	if (filePtr == NULL)
		return FALSE;

	// find length of file
	fseek(filePtr, 0, SEEK_END);
	fileLen = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	// read entire file into buffer
	buffer = new char [fileLen+1];
	fread(buffer, sizeof(char), fileLen, filePtr);

	// extract model file header from buffer
	modelHeader = (modelHeader_t*)buffer;

	// allocate vertex list
	vertexList = new vector_t [modelHeader->numXYZ * modelHeader->numFrames];

	numVertices = modelHeader->numXYZ;
	numFrames = modelHeader->numFrames;
	frameSize = modelHeader->framesize;

	for (j = 0; j < numFrames; j++)
	{
		frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize * j];

		vertexListPtr = (vector_t*)&vertexList[numVertices * j];
		for (i = 0; i < numVertices; i++)
		{
			vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
			vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
			vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
		}
	}

	numST = modelHeader->numST;

	st = new texCoord_t [numST];

	stPtr = (stIndex_t*)&buffer[modelHeader->offsetST];
	for (i = 0; i < numST; i++)
	{
		st[i].s = 0.0;
		st[i].t = 0.0;
	}

	numTriangles = modelHeader->numTris;
	triIndex = new mesh_t [numTriangles];

	// point to triangle indexes in buffer
	bufIndexPtr = (mesh_t*)&buffer[modelHeader->offsetTris];

	// create a mesh (triangle) list
	for (j = 0; j < numFrames; j++)         
	{
		// for all triangles in each frame
		for(i = 0; i < numTriangles; i++)
		{
			triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
			triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
			triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
			triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
			triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
			triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
		}
	}

	// close file and free memory
	fclose(filePtr);

	modelTex = NULL;
	currentFrame = 0;
	nextFrame = 1;
	interpol = 0.0;

	return 0;
}

// CMD2Model::LoadSkin()
// access: public
// desc: loads a skin for the model
int CMD2Model::LoadSkin(char *skinFile)
{
	int i;

	modelTex = LoadTexture(skinFile);

	if (modelTex != NULL)
		SetupSkin(modelTex);
	else
		return -1;

	for (i = 0; i < numST; i++)
	{
		st[i].s /= (float)modelTex->get_width();
		st[i].t /= (float)modelTex->get_height();
	}

	return 0;
}

// CMD2Model::SetTexture()
// access: public
// desc: sets a new texture object 
int CMD2Model::SetTexture(texture *tex)
{
	int i;

	if (tex != NULL)
	{
		free(modelTex);
		modelTex = tex;
	}
	else
		return -1;

	SetupSkin(modelTex);

	for (i = 0; i < numST; i++)
	{
		st[i].s /= (float)modelTex->get_width();
		st[i].t /= (float)modelTex->get_height();
	}

	return 0;
}

// CMD2Model::Animate()
// access: public
// desc: animates the model between the keyframes startFrame and endFrame
int CMD2Model::Animate(int startFrame, int endFrame, float percent)
{
	vector_t *vList;              // current frame vertices
	vector_t *nextVList;          // next frame vertices
	int i;                                  // index counter
	float x1, y1, z1;                  // current frame point values
	float x2, y2, z2;                  // next frame point values

	vector_t vertex[3]; 

	if ((startFrame > currentFrame))
		currentFrame = startFrame;

	if ((startFrame < 0) || (endFrame < 0))
		return -1;

	if ((startFrame >= numFrames) || (endFrame >= numFrames))
		return -1;

	if (interpol >= 1.0)
	{
		interpol = 0.0f;
		currentFrame++;
		if (currentFrame >= endFrame)
			currentFrame = startFrame;

		nextFrame = currentFrame + 1;

		if (nextFrame >= endFrame)
			nextFrame = startFrame;

	}

	vList = &vertexList[numVertices*currentFrame];
	nextVList = &vertexList[numVertices*nextFrame];

	glBindTexture(GL_TEXTURE_2D, modelTex->get_tex_id());
	glBegin(GL_TRIANGLES);
	for (i = 0; i < numTriangles; i++)
	{
		// get first points of each frame
		x1 = vList[triIndex[i].meshIndex[0]].point[0];
		y1 = vList[triIndex[i].meshIndex[0]].point[1];
		z1 = vList[triIndex[i].meshIndex[0]].point[2];
		x2 = nextVList[triIndex[i].meshIndex[0]].point[0];
		y2 = nextVList[triIndex[i].meshIndex[0]].point[1];
		z2 = nextVList[triIndex[i].meshIndex[0]].point[2];

		// store first interpolated vertex of triangle
		vertex[0].point[0] = x1 + interpol * (x2 - x1);
		vertex[0].point[1] = y1 + interpol * (y2 - y1);
		vertex[0].point[2] = z1 + interpol * (z2 - z1);

		// get second points of each frame
		x1 = vList[triIndex[i].meshIndex[2]].point[0];
		y1 = vList[triIndex[i].meshIndex[2]].point[1];
		z1 = vList[triIndex[i].meshIndex[2]].point[2];
		x2 = nextVList[triIndex[i].meshIndex[2]].point[0];
		y2 = nextVList[triIndex[i].meshIndex[2]].point[1];
		z2 = nextVList[triIndex[i].meshIndex[2]].point[2];

		// store second interpolated vertex of triangle
		vertex[2].point[0] = x1 + interpol * (x2 - x1);
		vertex[2].point[1] = y1 + interpol * (y2 - y1);
		vertex[2].point[2] = z1 + interpol * (z2 - z1);   

		// get third points of each frame
		x1 = vList[triIndex[i].meshIndex[1]].point[0];
		y1 = vList[triIndex[i].meshIndex[1]].point[1];
		z1 = vList[triIndex[i].meshIndex[1]].point[2];
		x2 = nextVList[triIndex[i].meshIndex[1]].point[0];
		y2 = nextVList[triIndex[i].meshIndex[1]].point[1];
		z2 = nextVList[triIndex[i].meshIndex[1]].point[2];

		// store third interpolated vertex of triangle
		vertex[1].point[0] = x1 + interpol * (x2 - x1);
		vertex[1].point[1] = y1 + interpol * (y2 - y1);
		vertex[1].point[2] = z1 + interpol * (z2 - z1);

		// calculate the normal of the triangle
		CalculateNormal(vertex[0].point, vertex[2].point, vertex[1].point);

		// render properly textured triangle
		glTexCoord2f(st[triIndex[i].stIndex[0]].s,
			st[triIndex[i].stIndex[0]].t);
		glVertex3fv(vertex[0].point);

		glTexCoord2f(st[triIndex[i].stIndex[2]].s ,
			st[triIndex[i].stIndex[2]].t);
		glVertex3fv(vertex[2].point);

		glTexCoord2f(st[triIndex[i].stIndex[1]].s,
			st[triIndex[i].stIndex[1]].t);
		glVertex3fv(vertex[1].point);
	}
	glEnd();

	interpol += percent;  // increase percentage of interpolation between frames

	return 0;
}

int CMD2Model::Animate(float percent) {
	return Animate(binome[modelState].beg, binome[modelState].end, percent);
}

// RenderFrame()
// desc: renders a single key frame
int CMD2Model::RenderFrame(int keyFrame)
{
	vector_t *vList;
	int i;

	// create a pointer to the frame we want to show
	vList = &vertexList[numVertices * keyFrame];

	if (modelTex != NULL)
		// set the texture
		glBindTexture(GL_TEXTURE_2D, modelTex->get_tex_id());

	// display the textured model with proper lighting normals
	glBegin(GL_TRIANGLES);
	for(i = 0; i < numTriangles; i++)
	{
		CalculateNormal(vList[triIndex[i].meshIndex[0]].point,
			vList[triIndex[i].meshIndex[2]].point,
			vList[triIndex[i].meshIndex[1]].point);

		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[0]].s,
			st[triIndex[i].stIndex[0]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[0]].point);

		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[2]].s ,
			st[triIndex[i].stIndex[2]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[2]].point);

		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[1]].s,
			st[triIndex[i].stIndex[1]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[1]].point);
	}
	glEnd();

	return 0;
}

// Unload()
// desc: unloads model data from memory
int CMD2Model::Unload()
{
	if (triIndex != NULL)
		free(triIndex);
	if (vertexList != NULL)
		free(vertexList);
	if (st != NULL)
		free(st);

	return 0;
}

// SetState()
// desc: set the model state
int CMD2Model::SetState(modelState_t state)
{
	if (modelState != state) {
		modelState = state;
		if (state == MODEL_JUMP)
			currentFrame = 72;
		else
			currentFrame = binome[modelState].beg;
		interpol = 0.0f;
	}
	return 0;
}

// GetState()
// desc: retrieve the model state
modelState_t CMD2Model::GetState()
{
	return modelState;
}

const char* CMD2Model::GetStateName() {
	switch (modelState) {
		case MODEL_IDLE :
			return "idle";
		case MODEL_RUN :
			return "run";
		case MODEL_SHOT_STAND :
			return "shot stand";
		case MODEL_SHOT_SHOULDER :
			return "shot shoulder";
		case MODEL_JUMP :
			return "jump";
		case MODEL_IDLE2 :
			return "idle2";
		case MODEL_SHOT_FALL :
			return "shot fall";
		case MODEL_IDLE3 :
			return "idle3";
		case MODEL_IDLE4 :
			return "idle4";
		case MODEL_CROUCH :
			return "crouch";
		case MODEL_CROUCH_CRAWL :
			return "crouch crawl";
		case MODEL_CROUCH_WEAPON :
			return "crouch weapon";
		case MODEL_DYING_KNEE :
			return "dying knee";
		case MODEL_DYING_BACK :
			return "dying back";
		case MODEL_DYING_FORWARD :
			return "dying forward";
		case MODEL_DYING_SLOW :
			return "dying slow";
		default :
			return "unknown";
	}
}
