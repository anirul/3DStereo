//////////////////////////////////////////////////////////////////////////
// MD2 class
//////////////////////////////////////////////////////////////////////////
// author	: Frederic DUBOUCHET
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// inpired from OpenGL Game programming

#ifndef __3DSTEREO_MD2_HEADER_DEFINED__
#define __3DSTEREO_MD2_HEADER_DEFINED__

#define MAX_FRAMES 512


// model animation states
enum modelState_t
{
	MODEL_IDLE = 0,
	MODEL_RUN = 1,
	MODEL_SHOT_STAND = 2,
	MODEL_SHOT_SHOULDER = 3,
	MODEL_JUMP = 4,
	MODEL_IDLE2 = 5,
	MODEL_SHOT_FALL = 6,
	MODEL_IDLE3 = 7,
	MODEL_IDLE4 = 8,
	MODEL_CROUCH = 9,
	MODEL_CROUCH_CRAWL = 10,
	MODEL_CROUCH_WEAPON = 11,
	MODEL_DYING_KNEE = 12,
	MODEL_DYING_BACK = 13,
	MODEL_DYING_FORWARD = 14,
	MODEL_DYING_SLOW = 15
};

struct begend {
	begend() : beg(0), end(0) {}
	begend(int b, int e) : beg(b), end(e) {}
	int beg;
	int end;
};

//texture_t *LoadTexture(char *filename);

/*
	Vector Functionality
*/
// a single vertex
typedef struct
{
   float point[3];
} vector_t;

vector_t operator-(vector_t a, vector_t b);
vector_t operator*(float f, vector_t b);
vector_t operator/(vector_t a, vector_t b);
vector_t operator+(vector_t a, vector_t b);

void CalculateNormal(float *p1, float *p2, float *p3);

// texture coordinate
typedef struct
{
	float s;
	float t;
} texCoord_t;

// texture coordinate index
typedef struct
{
	short s;
	short t;
} stIndex_t;

// info for a single frame point
typedef struct
{
	unsigned char v[3];
	unsigned char normalIndex;	// not used
} framePoint_t;

// information for a single frame
typedef struct
{
	float scale[3];
	float translate[3];
	char name[16];
	framePoint_t fp[1];
} frame_t;

// data for a single triangle
typedef struct
{
	unsigned short meshIndex[3];	// vertex indices
	unsigned short stIndex[3];		// texture coordinate indices
} mesh_t;

typedef struct
{
	int ident;			// identifies as MD2 file "IDP2"
	int version;		// mine is 8
	int skinwidth;		// width of texture
	int skinheight;		// height of texture
	int framesize;		// number of bytes per frame
	int numSkins;		// number of textures
	int numXYZ;			// number of points
	int numST;			// number of texture
	int numTris;		// number of triangles
	int numGLcmds;
	int numFrames;		// total number of frames
	int offsetSkins;	// offset to skin names (64 bytes each)
	int offsetST;		// offset of texture s-t values
	int offsetTris;		// offset of triangle mesh
	int offsetFrames;	// offset of frame data (points)
	int offsetGLcmds;	// type of OpenGL commands to use
	int offsetEnd;		// end of file
} modelHeader_t;


class CMD2Model
{
private:
	std::map<modelState_t, begend> binome; // begin and end of anims
	int numFrames;			 // number of model frames
	int numVertices;         // number of vertices
	int numTriangles;        // number of triangles
	int numST;               // number of skins
	int frameSize;           // size of each frame in bytes
	int currentFrame;        // current frame # in animation
	int nextFrame;           // next frame # in animation
	float interpol;          // percent through current frame
	mesh_t *triIndex;        // triangle list
	texCoord_t *st;          // texture coordinate list
	vector_t *vertexList;    // vertex list
	texture *modelTex;		 // texture data

	modelState_t modelState;	// current model animation state

	void SetupSkin(texture *thisTexture);

public:

	CMD2Model();        // constructor
	~CMD2Model();       // destructor

	// load model and skin/texture at the same time
	int Load(const char* modelFile, const char* skinFile);

	// load model only
	int LoadModel(char *modelFile);

	// load skin only
	int LoadSkin(char *skinFile);

	// set model's texture/skin
	int SetTexture(texture *tex);

	// render model with interpolation to get animation
	int Animate(int startFrame, int endFrame, float percent);
	int Animate(float percent);

	// render a single frame
	int RenderFrame(int keyFrame);

	// free memory of model
	int Unload();

	// set animation state of model
	int SetState(modelState_t state);	

	// retrieve animation state of model
	modelState_t GetState();
	const char* GetStateName();
};


#endif // __3DSTEREO_MD2_HEADER_DEFINED__
