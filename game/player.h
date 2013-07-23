//////////////////////////////////////////////////////////////////////////
// player Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// player handler hold the character file and physique informations.

#ifndef __3DSTEREO_PLAYER_HEADER_DEFINED__
#define __3DSTEREO_PLAYER_HEADER_DEFINED__

enum player_state {
	STATE_RUN,
	STATE_STAND,
	STATE_JUMP_UP,
	STATE_JUMP_DOWN
};

class player {
	vector3 m_pos;
	vector3 m_dir;
	vector3 m_velocity;
	vector3 m_delta;
	float m_deltat;
	CMD2Model* m_charmodel;
	level* m_level;
	player_state m_ps;
public:
	player(std::string& model, std::string& skin);
	~player();
	void check_key();
	void set_dir(vector3& dir);
	void set_level(level* p) { m_level = p; }
	vector3& velocity() { return m_velocity; }
	const vector3& pos() const { return m_pos; }
	const vector3& delta() const;
	void set_state(player_state ps);
	void animate(float deltat);
	void display();
};

#endif // __3DSTEREO_PLAYER_HEADER_DEFINED__
