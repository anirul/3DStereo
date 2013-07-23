//////////////////////////////////////////////////////////////////////////
// gamewin Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// Game window handler.

#ifndef __3DSTEREO_GAMEWIN_HEADER_DEFINED__
#define __3DSTEREO_GAMEWIN_HEADER_DEFINED__

class gamewin : public Iwin {
	// member functions
	int m_dx;
	int m_dy;
	level* m_level;
	font* m_font;
	timer* m_timer;
	camera* m_cam;
	player* m_player;
	vector2 m_mouse;
	bool m_stereo;
	std::string m_strlvl;
	float m_sep;
public :
	gamewin(bool stereo = false, std::string lvl = std::string("temp.lvl"), float sep = 40.0f);
	~gamewin();
	// glut virtual functions
	virtual void init();
	virtual void finish();
	virtual void display();
	virtual void reshape(int w, int h);
	virtual void mouse_move(int x, int y);
	virtual void mouse_event(int button, int state, int z, int y);
	virtual void idle();
	virtual void keyboard(unsigned char key, int z, int y);
};

#endif __3DSTEREO_GAMEWIN_HEADER_DEFINED__