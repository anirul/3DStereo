#include "main.h"

static void init() {
#ifdef _DEBUG
//	printf("glutwin::init<%p>\n", g_windesc);
#endif
	if (g_windesc)
		g_windesc->init();
}

static void finish() {
#ifdef _DEBUG
//	printf("glutwin::finish<%p>\n", g_windesc);
#endif
	if (g_windesc)
		g_windesc->finish();
}

static void display() {
#ifdef _DEBUG
//	printf("glutwin::display<%p>\n", g_windesc);
#endif
	static bool s_first = true;
	if (s_first) {
		s_first = false;
		init();
	} else if (g_windesc) {
		g_windesc->display();
	}
	glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y) {
#ifdef _DEBUG
//	printf("glutwin::keyboard<%p>(%d, %d, %d)\n", g_windesc, key, x, y);
#endif
	switch(key) {
		case 27 :
			finish();
			printf("\n");
			exit(0);
			break;
	}
	if (g_windesc)
		g_windesc->keyboard(key, x, y);
}

static void reshape(int w, int h) {
#ifdef _DEBUG
//	printf("glutwin::display<%p>(%d, %d)\n", g_windesc, w, h);
#endif
	if (g_windesc)
		g_windesc->reshape(w, h);
}

static void idle() {
#ifdef _DEBUG
//	printf("glutwin::idle<%p>\n", g_windesc);
#endif
	if (g_windesc)
		g_windesc->idle();
	glutPostRedisplay();
}

static void mouse_move(int x, int y) {
#ifdef _DEBUG
//	printf("glutwin::mouse_move<%p>(%d, %d)\n", g_windesc, x, y);
#endif
	if (g_windesc)
		g_windesc->mouse_move(x, y);
}

static void mouse_event(int button, int state, int x, int y) {
#ifdef _DEBUG
//	printf("glutwin::mouse_event<%p>(%d, %d, %d, %d)\n", 
//		g_windesc, button, state, x, y);
#endif
    if (g_windesc)
		g_windesc->mouse_event(button, state, x, y);
}

glutwin::glutwin(int ac, char **av, Iwin* windesc, bool fullscreen) {
	g_windesc = windesc;
	m_fullscreen = fullscreen;
	glutInit(&ac, av);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!fullscreen)
		glutInitWindowSize(640, 480);
	glutCreateWindow("glut window");
	if (fullscreen)
		glutFullScreen();
}

glutwin::~glutwin() {}

void glutwin::run() {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouse_move);
	glutMouseFunc(mouse_event);
	glutIdleFunc(idle);
	glutMainLoop();
}