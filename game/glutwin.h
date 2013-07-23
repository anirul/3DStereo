//////////////////////////////////////////////////////////////////////////
// glutwin Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// Window handling using glut.

#ifndef __3DSTEREO_GLUTWIN_HEADER_DEFINED__
#define __3DSTEREO_GLUTWIN_HEADER_DEFINED__

// interface for the window game engine
struct Iwin {
	virtual void init() = 0;
	virtual void display() = 0;
	virtual void idle() = 0;
	virtual void reshape(int w, int h) = 0;
	virtual void mouse_event(int button, int state, int x, int y) = 0;
	virtual void mouse_move(int x, int y) = 0;
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void finish() = 0;
};

// statics
static Iwin* g_windesc = NULL;
static void display();
static void idle();
static void reshape(int w, int h);
static void keyboard(unsigned char key, int x, int y);
static void mouse_move(int x, int y);
static void mouse_event(int button, int state, int x, int y);

// window class
class glutwin {
	bool m_fullscreen;
public :
	glutwin(int ac, char **av, Iwin* windesc, bool fullscreen = false);
    virtual ~glutwin();
	void run();
};

#endif // __3DSTEREO_GLUTWIN_HEADER_DEFINED__