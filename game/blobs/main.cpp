#include "main.h"

// arbitrary value
#define GRID_X 32
#define GRID_Y 32
#define GRID_Z 32
#define ISO_LEVEL 0.2
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef DWORD
#define DWORD uint32_t
#endif
#define MAX_VERTEX 100000
#define DTOR 0.0174532925
#define RTOD 57.2957795
#define PI 3.141592653589793238462643
#define CROSSPROD(p1,p2,p3) \
	p3.x = p1.y*p2.z - p1.z*p2.y; \
	p3.y = p1.z*p2.x - p1.x*p2.z; \
	p3.z = p1.x*p2.y - p1.y*p2.x

XYZ origin = {0.0,0.0,0.0};
double dtheta = 1;

struct CAMERA {
	XYZ vp;              /* View position           */
	XYZ vd;              /* View direction vector   */
	XYZ vu;              /* View up direction       */
	XYZ pr;              /* Point to rotate about   */
	double focallength;  /* Focal Length along vd   */
	double aperture;     /* Camera aperture         */
	double eyesep;       /* Eye separation          */
	int screenwidth,screenheight;
};

CAMERA camera;
XYZ spheres[32];
BYTE nbSpeheres = 0;
XYZ cvVertices[MAX_VERTEX];
DWORD dwNbVertices = 0;
double metagrille[GRID_X][GRID_Y][GRID_Z];
GRIDCELL gride[GRID_X - 1][GRID_Y - 1][GRID_Z - 1];
bool bStart = false;

void init(void);
void display(void);
void MakeGeometry(void);
void MakeLighting(void);
void update(void);
void keyboard(unsigned char key, int x, int y);
int main(int ac, char** av);
void CameraHome();
void Normalise(XYZ *);
void TranslateCamera(int ix,int iy);
void RotateCamera(int ix,int iy,int iz);
void HandleReshape(int w,int h);
void MakeCross(float x, float y , float z);

void HandleReshape(int w,int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	camera.screenwidth = w;
	camera.screenheight = h;
	printf("resize %d, %d\n", w, h);
}

void Normalise(XYZ *p)
{
	double length;

	length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
	if (length != 0) {
		p->x /= length;
		p->y /= length;
		p->z /= length;
	} else {
		p->x = 0;
		p->y = 0;
		p->z = 0;
	}
}

void TranslateCamera(int ix,int iy)
{
	XYZ vp,vu,vd;
	XYZ right;
	XYZ newvp,newr;
	double radians,delta;

	vu = camera.vu;
	Normalise(&vu);
	vp = camera.vp;
	vd = camera.vd;
	Normalise(&vd);
	CROSSPROD(vd,vu,right);
	Normalise(&right);
	radians = dtheta * PI / 180.0;
	delta = dtheta * camera.focallength / 90.0;

	camera.vp.x += iy * vu.x * delta;
	camera.vp.y += iy * vu.y * delta;
	camera.vp.z += iy * vu.z * delta;
	camera.pr.x += iy * vu.x * delta;
	camera.pr.y += iy * vu.y * delta;
	camera.pr.z += iy * vu.z * delta;

	camera.vp.x += ix * right.x * delta;
	camera.vp.y += ix * right.y * delta;
	camera.vp.z += ix * right.z * delta;
	camera.pr.x += ix * right.x * delta;
	camera.pr.y += ix * right.y * delta;
	camera.pr.z += ix * right.z * delta;
}

void RotateCamera(int ix,int iy,int iz)
{
	XYZ vp,vu,vd;
	XYZ right;
	XYZ newvp,newr;
	double radius,dd,radians;
	double dx,dy,dz;

	vu = camera.vu;
	Normalise(&vu);
	vp = camera.vp;
	vd = camera.vd;
	Normalise(&vd);
	CROSSPROD(vd,vu,right);
	Normalise(&right);
	radians = dtheta * PI / 180.0;

	/* Handle the roll */
	if (iz != 0) {
		camera.vu.x += iz * right.x * radians;
		camera.vu.y += iz * right.y * radians;
		camera.vu.z += iz * right.z * radians;
		Normalise(&camera.vu);
		return;
	}

	/* Distance from the rotate point */
	dx = camera.vp.x - camera.pr.x;
	dy = camera.vp.y - camera.pr.y;
	dz = camera.vp.z - camera.pr.z;
	radius = sqrt(dx*dx + dy*dy + dz*dz);

	/* Determine the new view point */
	dd = radius * radians;
	newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
	newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
	newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
	Normalise(&newvp);
	camera.vp.x = camera.pr.x + radius * newvp.x;
	camera.vp.y = camera.pr.y + radius * newvp.y;
	camera.vp.z = camera.pr.z + radius * newvp.z;

	/* Determine the new right vector */
	newr.x = camera.vp.x + right.x - camera.pr.x;
	newr.y = camera.vp.y + right.y - camera.pr.y;
	newr.z = camera.vp.z + right.z - camera.pr.z;
	Normalise(&newr);
	newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
	newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
	newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

	camera.vd.x = camera.pr.x - camera.vp.x;
	camera.vd.y = camera.pr.y - camera.vp.y;
	camera.vd.z = camera.pr.z - camera.vp.z;
	Normalise(&camera.vd);

	/* Determine the new up vector */
	CROSSPROD(newr,camera.vd,camera.vu);
	Normalise(&camera.vu);

}

void CameraHome()
{
	camera.aperture = 20.0;
	camera.focallength = 1.0;
	camera.eyesep = camera.focallength / 20.0;
	camera.pr = origin;


	camera.vp.x = 0;
	camera.vp.y = 0; 
	camera.vp.z = camera.focallength;
	camera.vd.x = 0;
	camera.vd.y = 0;
	camera.vd.z = -1;

	camera.vu.x = 0;  
	camera.vu.y = 1; 
	camera.vu.z = 0;
}

void init(void) {

	// init gl stuff
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glShadeModel(GL_SMOOTH);    
	glDisable(GL_DITHER);
	glDisable(GL_CULL_FACE);

	glLineWidth(1.0);
	glPointSize(1.0);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glFrontFace(GL_CW);
	glClearColor(0.3,0.3,0.3,0.0);         /* Background colour */
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	// init the array
	bStart = true;
	for (int jz = 0; jz < (GRID_Z - 1); ++ jz)
	{
		for (int jy = 0; jy < (GRID_Y - 1); ++ jy)
		{
			for (int jx = 0; jx < (GRID_X - 1); ++ jx)
			{
				GRIDCELL cell;
				int counter = 0;	
				// 0
				cell.p[counter].x = 
					(float)(jx / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)(jy / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)(jz / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 1
				cell.p[counter].x = 
					(float)((jx + 1) / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)(jy / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)(jz / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 2
				cell.p[counter].x = 
					(float)((jx + 1) / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)((jy + 1) / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)(jz / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 3
				cell.p[counter].x = 
					(float)(jx / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)((jy + 1) / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)(jz / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 4
				cell.p[counter].x = 
					(float)(jx / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)(jy / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)((jz + 1) / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 5
				cell.p[counter].x = 
					(float)((jx + 1) / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)(jy / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)((jz + 1) / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 6
				cell.p[counter].x = 
					(float)((jx + 1) / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)((jy + 1) / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)((jz + 1) / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				++counter;
				// 7
				cell.p[counter].x = 
					(float)(jx / (GRID_X / 2.0)) - 1.0f;
				cell.p[counter].y = 
					(float)((jy + 1) / (GRID_Y / 2.0)) - 1.0f;
				cell.p[counter].z = 
					(float)((jz + 1) / (GRID_Z / 2.0)) - 1.0f;
				cell.val[counter] = 0.0;
				// fill the gride
				gride[jx][jy][jz] = cell;
			}
		}
	}
}

void update(void) {
	// if no spheres nothing todo.
	static float frame = 0.0;
	if (!nbSpeheres) 
	{
		return;
	} else {
		frame += 0.05;
		spheres[0].x = 0.2f * (float)sin(frame * 1.0f) + 0.5f;
		spheres[0].y = 0.2f * (float)cos(frame * 1.0f) + 0.5f;
		spheres[1].y = 0.2f * (float)sin(frame * 0.7f) + 0.5f;
		spheres[1].z = 0.2f * (float)cos(frame * 0.7f) + 0.5f;
		spheres[2].z = 0.2f * (float)sin(frame * 1.3f) + 0.5f;
		spheres[2].x = 0.2f * (float)cos(frame * 1.3f) + 0.5f;
	}

	DWORD dwNbVertex = 0;
	// (from dake calodox)
	// modified to fill my needs
	double diffx, diffy, diffz;
	double dist;
	// calculate the metasphere
	for (int gz=0;gz<GRID_Z;gz++)                
	{
		for (int gy=0;gy<GRID_Y;gy++)
		{
			for (int gx=0;gx<GRID_X;gx++)       
			{
				//efface la grille 
				metagrille[gx][gy][gz]=0.0;
				//				double s = gy*gy;
				//				metagrille[gx][gy][gz] = s != 0 ? 1/s : 0;
				for (int n = 0; n < nbSpeheres; ++n)
				{
					diffx= gx - ((spheres[n].x + 1.0) * (GRID_X / 2.0)); 
					diffy= gy - ((spheres[n].y + 1.0) * (GRID_Y / 2.0));
					diffz= gz - ((spheres[n].z + 1.0) * (GRID_Z / 2.0));
					dist = 1.0/
						((diffx * diffx) + (diffy * diffy) + (diffz * diffz));
					metagrille[gx][gy][gz] += dist;
				}
			}
		}
	}

	// TODO add an algo to go faster HERE!!!
	for (int jz = 0; jz < (GRID_Z - 1); ++ jz)
	{
		for (int jy = 0; jy < (GRID_Y - 1); ++ jy)
		{
			for (int jx = 0; jx < (GRID_X - 1); ++ jx)
			{
				gride[jx][jy][jz].val[0] = metagrille[jx][jy][jz];
				gride[jx][jy][jz].val[1] = metagrille[jx + 1][jy][jz];
				gride[jx][jy][jz].val[2] = metagrille[jx + 1][jy + 1][jz];
				gride[jx][jy][jz].val[3] = metagrille[jx][jy + 1][jz];
				gride[jx][jy][jz].val[4] = metagrille[jx][jy][jz + 1];
				gride[jx][jy][jz].val[5] = metagrille[jx + 1][jy][jz + 1];
				gride[jx][jy][jz].val[6] = metagrille[jx + 1][jy + 1][jz + 1];
				gride[jx][jy][jz].val[7] = metagrille[jx][jy + 1][jz + 1];
				DWORD newVert = Polygonise(
					gride[jx][jy][jz], 
					ISO_LEVEL, 
					(TRIANGLE*)&cvVertices[dwNbVertex]) * 3;

				// make the Normals calculation
				for (DWORD pu = 0; pu < newVert; ++pu)
				{
					float vect[3] = {0.0f, 0.0f, 0.0f};
					for (int no = 0; no < nbSpeheres; ++no)
					{
						float dx, dy, dz;
						dx = cvVertices[dwNbVertex + pu].x - spheres[no].x;
						dy = cvVertices[dwNbVertex + pu].y - spheres[no].y;
						dz = cvVertices[dwNbVertex + pu].z - spheres[no].z;
						double mdist = 1.0 / sqrt((dx * dx) + (dy * dy) + (dz * dz));
						vect[0] += (float)(dx * mdist);
						vect[1] += (float)(dy * mdist);
						vect[2] += (float)(dz * mdist);
					}
					// Normalize
					float d = sqrt(
						(vect[0] * vect[0]) +
						(vect[1] * vect[1]) +
						(vect[2] * vect[2]));
					cvVertices[dwNbVertex + pu].nx = vect[0] / d;
					cvVertices[dwNbVertex + pu].ny = vect[1] / d;
					cvVertices[dwNbVertex + pu].nz = vect[2] / d;
				}
				dwNbVertex += newVert;
			}
		}
	}
	dwNbVertices = dwNbVertex;
}

void display() {
	
	double nearp = 0.1;
	double farp = 10000;
	XYZ r;
	double dist, ratio, radians, scale, wd2, ndfl;
	double left, right, top, bottom;
	
	/* Clip to avoid extreme stereo */
#ifdef STEREO_3D
	nearp = camera.focallength / 5;
#endif

	/* Misc stuff */
	ratio  = camera.screenwidth / (double)camera.screenheight;
	radians = DTOR * camera.aperture / 2;
	wd2     = nearp * tan(radians);
	ndfl    = nearp / camera.focallength;

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef STEREO_3D

		update();

		/* Derive the two eye positions */
		CROSSPROD(camera.vd,camera.vu,r);
		Normalise(&r);
		r.x *= camera.eyesep / 2.0;
		r.y *= camera.eyesep / 2.0;
		r.z *= camera.eyesep / 2.0;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
		right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
		top    =   wd2;
		bottom = - wd2;
		glFrustum(left,right,bottom,top,nearp,farp);

		glMatrixMode(GL_MODELVIEW);
		glDrawBuffer(GL_BACK);

#ifdef INVERT_STEREO
		glViewport (
			camera.screenwidth / 2, 0, 
			camera.screenwidth / 2, camera.screenheight);
#else
		glViewport (
			0, 0, 
			camera.screenwidth / 2, camera.screenheight);
#endif


		glLoadIdentity();
		gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
			camera.vp.x + r.x + camera.vd.x,
			camera.vp.y + r.y + camera.vd.y,
			camera.vp.z + r.z + camera.vd.z,
			camera.vu.x,camera.vu.y,camera.vu.z);

		glTranslatef(-0.5f, -0.5f, -2.0f);

		MakeLighting();
		MakeGeometry();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
		right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
		top    =   wd2;
		bottom = - wd2;
		glFrustum(left,right,bottom,top,nearp,farp);

		glMatrixMode(GL_MODELVIEW);
		glDrawBuffer(GL_BACK);

#ifdef INVERT_STEREO
		glViewport (
			0, 0, 
			camera.screenwidth / 2, camera.screenheight);
#else
		glViewport (
			camera.screenwidth / 2, 0, 
			camera.screenwidth / 2, camera.screenheight);
#endif


//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
			camera.vp.x - r.x + camera.vd.x,
			camera.vp.y - r.y + camera.vd.y,
			camera.vp.z - r.z + camera.vd.z,
			camera.vu.x,camera.vu.y,camera.vu.z);

		glTranslatef(-0.5f, -0.5f, -2.0f);

		MakeLighting();
		MakeGeometry();

		glutSwapBuffers();
		glutPostRedisplay();

#else

		update();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		left  = - ratio * wd2;
		right =   ratio * wd2;
		top    =   wd2;
		bottom = - wd2;
		glFrustum(left,right,bottom,top,nearp,farp);
		
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		gluLookAt(camera.vp.x,camera.vp.y,camera.vp.z,
			camera.vp.x + camera.vd.x,
			camera.vp.y + camera.vd.y,
			camera.vp.z + camera.vd.z,
			camera.vu.x,camera.vu.y,camera.vu.z);

		glTranslatef(-0.5f, -0.5f, -2.0f);

		MakeLighting();
		MakeGeometry();

		glutSwapBuffers();
		glutPostRedisplay();

#endif // STEREO_3D
}

void MakeLighting(void)
{
	GLfloat fullambient[4] = {0.1,0.1,0.1,0.0};
	GLfloat position[4] = {-5.0,5.0,5.0,0.0};
	GLfloat ambient[4]  = {0.0,0.3,0.3,0.0};
	GLfloat diffuse[4]  = {0.0,0.6,0.6,0.0};
	GLfloat specular[4] = {0.0,1.0,1.0,0.0};

	/* Turn off all the lights */
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

	/* Turn on the appropriate lights */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,fullambient);
	glLightfv(GL_LIGHT0,GL_POSITION,position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHT0);

	/* Sort out the shading algorithm */
	glShadeModel(GL_SMOOTH);

	/* Turn lighting on */
	glEnable(GL_LIGHTING);
}

void MakeCross(float x, float y , float z) {
	glBegin(GL_LINES);
		glVertex3f(x - 10.0f, y, z);
		glVertex3f(x + 10.0f, y, z);
		glVertex3f(x, y - 10.0f, z);
		glVertex3f(x, y + 10.0f, z);
		glVertex3f(x, y, z - 10.0f);
		glVertex3f(x, y, z + 10.0f);
	glEnd();
}

void MakeGeometry() {

	glBegin(GL_TRIANGLES);
	for (DWORD i= 0; i < dwNbVertices; ++i) {
		float* pv = &(cvVertices[i].x);
		float* nv = &(cvVertices[i].nx);
		glNormal3fv(nv);
		glVertex3fv(pv);
	}
	glEnd();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27 :
			// supposed to quit
			exit(0);
			break;
		case 'h':                           /* Go home     */
		case 'H':
			CameraHome();
			break;
	}
}

int main(int ac, char** av) {

	spheres[0].x = 0.5f;
	spheres[0].y = 0.5f;
	spheres[0].z = 0.5f;

	spheres[1].x = 0.5f;
	spheres[1].y = 0.5f;
	spheres[1].z = 0.5f;

	spheres[2].x = 0.5f;
	spheres[2].y = 0.5f;
	spheres[2].z = 0.5f;

	nbSpeheres = 3;

	glutInit(&ac, av);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize(640, 480);
	glutCreateWindow("GL blobs viewer");
	glutFullScreen();
	
	init();
	CameraHome();
	update();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(HandleReshape);
	glutMainLoop();
	return 0;
}
