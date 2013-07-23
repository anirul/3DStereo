#include "main.h"

using std::string;
using std::map;

static const char* filename=0;
static const char* camera=0;
static Lib3dsFile *file=0;
static float current_frame=0.0;
static int gl_width;
static int gl_height;
static int camera_menu_id=0;
static int halt=0;
typedef map<string, tga_tex> mnt_t;
static mnt_t mnt;

void init(void);
void reinit(void);
void render_node(Lib3dsNode *node);
void keyboard(unsigned char key, int x, int y);
void menustate(int item);
void menu(int item);
void display(void);
void reshape (int w, int h);

void init(void) {
	// Init matrix
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	// init contextual menu
	glutMenuStateFunc(menustate);
	glutCreateMenu(menu);
	glutAddMenuEntry("[o]	Open a Scene...", 'o');
	glutAddMenuEntry("", 0);
	glutAddMenuEntry("[Esc]	Quit", 27);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reinit(void) {
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glDisable(GL_NORMALIZE);
	//glPolygonOffset(1.0, 2);


	file=lib3ds_file_load(filename);
	if (!file) {
		puts("***ERROR*** Loading 3DS file failed.");
		exit(1);
	}
	if (!file->cameras) {
		puts("***ERROR*** No Camera found.");
		lib3ds_file_free(file);
		file=0;
		exit(1);
	}
	if (!camera) {
		camera=file->cameras->name;
	}
	lib3ds_file_eval(file,0);
}

void render_node(Lib3dsNode *node)
{
	ASSERT(file);

	{
		Lib3dsNode *p;
		for (p=node->childs; p!=0; p=p->next) {
			render_node(p);
		}
	}
	if (node->type==LIB3DS_OBJECT_NODE) {
		if (strcmp(node->name,"$$$DUMMY")==0) {
			return;
		}

		if (!node->user.d) {
			Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
			ASSERT(mesh);
			if (!mesh) {
				return;
			}

			node->user.d=glGenLists(1);
			glNewList(node->user.d, GL_COMPILE);

			{
				unsigned p;
				Lib3dsVector* normalL = (Lib3dsVector*)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

				{
					Lib3dsMatrix M;
					lib3ds_matrix_copy(M, mesh->matrix);
					lib3ds_matrix_inv(M);
					glMultMatrixf(&M[0][0]);
				}
				lib3ds_mesh_calculate_normals(mesh, normalL);

				for (p=0; p<mesh->faces; ++p) {
					Lib3dsFace *f=&mesh->faceL[p];
					Lib3dsMaterial *mat=0;
					if (f->material[0]) {
						mat=lib3ds_file_material_by_name(file, f->material);
						if (mat->texture1_map.name[0]) {
							string str = mat->texture1_map.name;
							map<string, tga_tex>::iterator ite = mnt.find(str);
							if (ite == mnt.end()) {
								char str2[256];
								memset(str2, 0, 256);
								memcpy(str2, str.c_str(), strlen(str.c_str()));
								str2[strlen(str.c_str()) - 3] = 't';
								str2[strlen(str.c_str()) - 2] = 'g';
								str2[strlen(str.c_str()) - 1] = 'a';
								printf("%s -> %s\n", str.c_str(), str2);
								mnt.insert(mnt_t::value_type(str, tga_tex(str2)));
							}
							glBindTexture(GL_TEXTURE_2D, (ite->second).GetglID());
						} else {
							glBindTexture(GL_TEXTURE_2D, 0);
						}
	//					lib3ds_material_dump(mat);
					}

					if (mat) {
						static GLfloat a[4]={0,0,0,1};
						float s;
						glMaterialfv(GL_FRONT, GL_AMBIENT, a);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
						s = pow(2.0f, 10.0f*mat->shininess);
						if (s>128.0) {
							s=128.0;
						}
						glMaterialf(GL_FRONT, GL_SHININESS, s);
					}
					else {
						Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
						Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
						Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
						glMaterialfv(GL_FRONT, GL_AMBIENT, a);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
						glMaterialfv(GL_FRONT, GL_SPECULAR, s);
					}
					{
						int i;
						glBegin(GL_TRIANGLES);
						glNormal3fv(f->normal);
						for (i=0; i<3; ++i) {
							assert(mesh);
							if (mesh->texelL) {
								glTexCoord2fv(mesh->texelL[f->points[i]]);
							}
							glNormal3fv(normalL[3*p+i]);
							glVertex3fv(mesh->pointL[f->points[i]].pos);
						}
						glEnd();
					}
				}

				free(normalL);
			}

			glEndList();
		}

		if (node->user.d) {
			Lib3dsObjectData *d;

			glPushMatrix();
			d=&node->data.object;
			glMultMatrixf(&node->matrix[0][0]);
			glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
			glCallList(node->user.d);
			//      glutSolidSphere(50.0, 20,20);
			glPopMatrix();
		}
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27 :
			// supposed to quit
			exit(0);
			break;
		case 'o' :
		case 'O' :
			{
				OPENFILENAME ofn;       // common dialog box structure
				char szFile[260];       // buffer for file name

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = szFile;
				//
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				//
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "3D Studio file\0*.3DS\0All\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 

				if (GetOpenFileName(&ofn)==TRUE) {
					filename = ofn.lpstrFile;
					reinit();
				}
			}
			break;
		default :
			break;
	}
}

void menustate(int item) {
	// put in menu (pause the drawing?)
}

void menu(int item) {
	keyboard((unsigned char)item, 0, 0);
}

void display(void) {
	if (file == 0) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
/*		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_POLYGON);
			glVertex3f(0.25, 0.25, 0.0);
			glVertex3f(0.75, 0.25, 0.0);
			glVertex3f(0.75, 0.75, 0.0);
			glVertex3f(0.25, 0.75, 0.0);
		glEnd();*/
		glutSwapBuffers();
	} else {
		Lib3dsNode *c,*t;
		Lib3dsMatrix M;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		if (!file) {
			return;
		}
		c=lib3ds_file_node_by_name(file, camera, LIB3DS_CAMERA_NODE);
		t=lib3ds_file_node_by_name(file, camera, LIB3DS_TARGET_NODE);
		if (!c || !t) {
			return;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( c->data.camera.fov, 1.0*gl_width/gl_height, 1.0, 20000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1.0,0,0);

		{
			GLfloat a[] = {0.0f, 0.0f, 0.0f, 1.0f};
			GLfloat c[] = {1.0f, 1.0f, 1.0f, 1.0f};
			GLfloat p[] = {0.0f, 0.0f, 0.0f, 1.0f};
			Lib3dsLight *l;

			int li=GL_LIGHT0;
			for (l=file->lights; l; l=l->next) {
				glEnable(li);

				glLightfv(li, GL_AMBIENT, a);
				glLightfv(li, GL_DIFFUSE, c);
				glLightfv(li, GL_SPECULAR, c);

				p[0] = l->position[0];
				p[1] = l->position[1];
				p[2] = l->position[2];
				glLightfv(li, GL_POSITION, p);

				if (!l->spot_light) {
					continue;
				}

				p[0] = l->spot[0] - l->position[0];
				p[1] = l->spot[1] - l->position[1];
				p[2] = l->spot[2] - l->position[2];      
				glLightfv(li, GL_SPOT_DIRECTION, p);
				++li;
			}
		}

		lib3ds_matrix_camera(M, c->data.camera.pos, t->data.target.pos, c->data.camera.roll);
		glMultMatrixf(&M[0][0]);

		{
			Lib3dsNode *p;
			for (p=file->nodes; p!=0; p=p->next) {
				render_node(p);
			}
		}

		if (!halt) {
			current_frame+=1.0;
			if (current_frame>file->frames) {
				current_frame=0;
			}
			lib3ds_file_eval(file, current_frame);
			glutSwapBuffers();
			glutPostRedisplay();
		}
	}
}

void reshape (int w, int h)
{
	gl_width=w;
	gl_height=h;
	glViewport(0,0,w,h);
}

int main(int ac, char** av) {
	glutInit(&ac, av);
#ifndef STEREO_3D
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO);
#endif // STEREO_3D
	glutInitWindowSize(640, 480);
	glutCreateWindow("GL Scene viewer");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}