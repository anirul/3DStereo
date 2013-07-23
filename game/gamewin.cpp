#include "main.h"

gamewin::gamewin(bool stereo /* = false */, 
				 std::string lvl /* = std::string("temp.lvl" */,
				 float sep) {
	m_level = 0;
	m_timer = 0;
	m_font = 0;
	m_cam = 0;
	m_player = 0;
	m_stereo = stereo;
	m_strlvl = lvl;
	m_sep = sep;
}

gamewin::~gamewin() {
	finish();
}

void gamewin::init() {
#ifdef _DEBUG
	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);
#else
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#endif
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	m_player = new player(
		std::string("models\\alita\\tris.md2"),
		std::string("models\\alita\\alita.pcx"));
	m_cam = new camera(
		vector3(0.0f, 45.0f, 75.0f), 
		vector3(0.0f, 30.0f, 0.0f),
		m_sep);

	try {
		m_level = new level(m_strlvl);
		m_player->set_level(m_level);
	} catch (std::string& str) {
		printf(str.c_str());
		exit(0);
	}
	m_timer = new timer;
	if (!m_timer->init()) {
		printf("Error no performance counter!");
		finish();
		exit(0);
	}
}

void gamewin::finish() {
	if (m_level) {
		delete m_level;
		m_level = 0;
	}
	if (m_timer) {
		delete m_timer;
		m_timer = 0;
	}
	if (m_font) {
		delete m_font;
		m_font = 0;
	}
	if (m_cam) {
		delete m_cam;
		m_cam = 0;
	}
	if (m_player) { 
		delete m_player;
		m_player = 0;
	}
}

void gamewin::mouse_move(int x, int y) {
	vector3 vec;
	vec.x = -(x - m_mouse.x) * 0.01f;
	vec.y = (y - m_mouse.y) * 0.5f;
	vec.z = 0.0f;
	m_mouse.x = x;
	m_mouse.y = y;
	m_cam->rotate(vec);
	m_player->set_dir(m_cam->dir());
}

void gamewin::mouse_event(int button, int state, int x, int y) {
	if (!state) {
		m_mouse.x = x;
		m_mouse.y = y;
	}
}

void gamewin::display() {
	if (m_timer) {
		std::string str("\rFPS : ");
		char temp[70];
		vector3 vec = m_player->delta();
		memset(temp, 0, 70);
		sprintf(temp, "%2.2f", m_timer->get_fps());
		str += temp;
		str += " sec time : ";
		memset(temp, 0, 70);
		sprintf(temp, "%2.2f", m_timer->now());
		str += temp;
		str += " current position : ";
		sprintf(temp, "(%2.2f, %2.2f, %2.2f)\r", vec.x, vec.y, vec.z);
        str += temp;		
		printf(str.c_str());
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_stereo) {

		// draw left
		glViewport(0, 0, (GLsizei)m_dx / 2, (GLsizei)m_dy);
		if (m_cam) {
			m_cam->ratio(m_dx / 2, m_dy);
			m_cam->set_left();
		}
		glPushMatrix();
		glScalef(WSCALE, WSCALE, WSCALE);
		if (m_level)
			m_level->display();
		glPopMatrix();

		glPushMatrix();
		if (m_player)
			m_player->display();
		glPopMatrix();

		// draw right
		glViewport((GLsizei)m_dx / 2, 0, (GLsizei)m_dx / 2, (GLsizei)m_dy);
		if (m_cam) {
			m_cam->ratio(m_dx / 2, m_dy);
			m_cam->set_right();
		}
		glPushMatrix();
		glScalef(WSCALE, WSCALE, WSCALE);
		if (m_level)
			m_level->display();
		glPopMatrix();

		glPushMatrix();
		if (m_player)
			m_player->display();
		glPopMatrix();

	} else {
		glViewport(0,0,(GLsizei)m_dx,(GLsizei)m_dy);
		if (m_cam) {
			m_cam->ratio(m_dx, m_dy);
			m_cam->set_center();
		}

		glPushMatrix();
			glScalef(WSCALE, WSCALE, WSCALE);
			if (m_level)
				m_level->display();
		glPopMatrix();

		glPushMatrix();
			if (m_player)
				m_player->display();
		glPopMatrix();
	}
	glFlush();
}

void gamewin::reshape(int w, int h) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_dx = w;
	m_dy = h;
}

// game loop
void gamewin::idle() {
	float deltat = 0.0f;
	if (m_timer)
		deltat = m_timer->get_elapsed_seconds();
	// gather input
	if (m_player)
		m_player->check_key();
	// move player
	try {
		if (m_player)
			m_player->animate(deltat);
	} catch (block_hit_type bht) {
		switch (bht) {
			case BHT_END :
				printf("\n\n *** CONGRATULATION YOU ESCAPED THE MAZE ***\n\n");
				printf("map\t: %s\n", m_strlvl.c_str());
				printf("time\t: %3.2f [s]\n", m_timer->now());
				exit(0);
				break;
			case BHT_DIE :
				{
					timer* t_timer = m_timer;
					m_timer = 0;
					finish();
					init();
					if (m_timer) delete m_timer;
					m_timer = t_timer;
				}
				break;
			default :
				throw "unknown error in gamewin::idle";
		}
	}
	if (m_cam && m_player) {
		m_cam->translate(m_player->delta());
		glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&(m_cam->pos().vec));
	}
	// will call display and redraw
}

void gamewin::keyboard(unsigned char key, int z, int y) {}

