//////////////////////////////////////////////////////////////////////////
// camera Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// camera handling class (stereo included).

#ifndef __3DSTEREO_CAMERA_HEADER_DEFINED__
#define __3DSTEREO_CAMERA_HEADER_DEFINED__

class camera {
	vector3 m_pos;
	vector3 m_target;
	vector3 m_up;
	float m_fov;
	float m_ratio;
	float m_near_plane;
	float m_far_plane;
	float m_eye_separation;
public:
	camera(vector3& pos, vector3& target, float sep = 40.0f);
	void translate(const vector3& vec);
	void rotate(vector3& vec);
	void ratio(int w, int h) { m_ratio = (float)w / (float)h; }
	vector3& dir();
	const vector3& pos() const { return m_pos; }
	float& fov() { return m_fov; }
	vector3& pos() { return m_pos; }
	void set_center();
	void set_right();
	void set_left();
};

#endif // __3DSTEREO_CAMERA_HEADER_DEFINED__