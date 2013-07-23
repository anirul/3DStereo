#include "main.h"

player::player(std::string& model, std::string& skin) {
	m_dir = vector3(0.0f, 0.0f, -1.0f);
	m_velocity = vector3(0.0f);
	m_delta = vector3(0.0f);
	m_pos = vector3(0.0f);
	m_deltat = 0.0f;
	m_level = 0;
	m_charmodel = new CMD2Model;
	m_charmodel->Load(model.c_str(), skin.c_str());
	this->set_state(STATE_STAND);
}

player::~player() {
	if (m_charmodel) {
		delete m_charmodel;
		m_charmodel = NULL;
	}
}

void player::check_key() {
	vector3 up(0.0f, 1.0f, 0.0f);
	vector3 right;
	vector3 dir = m_dir;
	dir.y = 0.0f;
	dir.Normalize();
	right = (dir % up);
	right.Normalize();
	vector3 left = (up % dir).Normalize();
	bool keyA = down('A');
	bool keyD = down('D');
	bool keyW = down('W');
	bool keyS = down('S');
	bool keyspace = down(VK_SPACE);
	if ((m_ps != STATE_JUMP_UP) && (m_ps != STATE_JUMP_DOWN)) {
		if (keyA || keyD || keyS || keyW || keyspace) {
			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;
			if (keyA) {
				m_velocity += left * 150;
				set_state(STATE_RUN);
			} else if (keyD) {
				m_velocity += right * 150;
				set_state(STATE_RUN);
			}
			if (keyW) {
				m_velocity += dir * 150;
				set_state(STATE_RUN);
			} else if (keyS) {
				m_velocity -= dir * 150;
				set_state(STATE_RUN);
			}
			if (keyspace) {
				m_velocity += up * 300;
				m_velocity += dir * 50;
				set_state(STATE_JUMP_UP);
			}
		} else {
			m_velocity = vector3(0.0f);
			set_state(STATE_STAND);
		}
	}
}

void player::set_dir(vector3& dir) {
	m_dir = dir.Normalize();
}

void player::animate(float deltat) {
	const vector3 gravity(0.0f, -9.81, 0.0f);
	m_deltat = deltat;
	// calculate new speed (increased by gravity)
	m_velocity += gravity * deltat * 150;
	// calculate movement (due to velocity)
	m_delta = m_velocity * deltat;
	if (m_level)
		m_delta = (m_level->hit(m_pos * (1 / WSCALE), m_delta * (1 / WSCALE))) * WSCALE;
	// hit the ground stop falling
	if (m_delta.y == 0.0f && m_ps == STATE_JUMP_UP)
		m_ps = STATE_STAND;
	// on the ground don't increase falling speed
	if (m_delta.y == 0.0f)
		m_velocity.y = 0.0f;
	m_pos += m_delta;
}

const vector3& player::delta() const {
	return m_delta; 
}

void player::set_state(player_state ps) {
	m_ps = ps;
	switch(ps) {
		case STATE_STAND :
			m_charmodel->SetState(MODEL_IDLE);
			break;
		case STATE_RUN :
			m_charmodel->SetState(MODEL_RUN);
			break;
		case STATE_JUMP_UP :
			m_charmodel->SetState(MODEL_JUMP);
			break;
		case STATE_JUMP_DOWN :
			m_charmodel->SetState(MODEL_JUMP);
			break;
	}
}

void player::display() {
	glPushMatrix();
		glTranslatef(m_pos.x, m_pos.y + 24.0, m_pos.z);
		vector3 dir = m_dir;
		dir.y = 0.0f;
		dir = dir.Normalize();
		float angle = RTOD * atan2f(dir.z, dir.x);
		glRotatef(angle, 0.0f, -1.0f, 0.0f);
		glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
//		glColor3f(1.0, 1.0, 1.0);
		if (m_charmodel) {
			m_charmodel->Animate(m_deltat * 7.5f);
			m_deltat = 0.0f;
		}
	glPopMatrix();
}
