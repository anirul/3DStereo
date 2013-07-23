#include "main.h"

camera::camera(vector3& pos, vector3& target, float sep) {
	m_pos = pos;
	m_target = target;
	m_fov = 45.0f;
	m_ratio = 4.0f / 3.0f;
	m_far_plane = 10000.0f;
	m_near_plane = 40.0f;
	m_eye_separation = (target - pos).Length() / sep;
	m_up = vector3(0.0f, 1.0f, 0.0f);
	vector3 right = m_up % (target - pos);
	m_up = (target - pos) % right;
	m_up = m_up.Normalize();
}

void camera::translate(const vector3& vec) {
	m_pos += vec;
	m_target += vec;
}

void camera::rotate(vector3& vec) {
	const float m_maxz = 100.0f;
	const float m_minz = 5.0f;
	// X
	vector3 pos = m_pos - m_target;
	matrix rotY(
		cos(vec.x), 0.0f, -sin(vec.x), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(vec.x), 0.0f, cos(vec.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	pos = pos * rotY;
	pos += m_target;
	// Z
	vector3 ta2po = pos - m_target;
	ta2po = ta2po.Normalize() * (ta2po.Length() + vec.z);
	if (ta2po.Length() > m_maxz)
		ta2po = ta2po.Normalize() * m_maxz;
	if (ta2po.Length() < m_minz)
		ta2po = ta2po.Normalize() * m_minz;
	// Y
	float t2pl = ta2po.Length();
	ta2po.y += vec.y;
	ta2po = ta2po.Normalize() * t2pl;
	m_pos = ta2po + m_target;
	// recalculate up
	m_up = vector3(0.0f, 1.0f, 0.0f);
	vector3 right = m_up % (m_target - m_pos);
	m_up = (m_target - m_pos) % right;
	m_up = m_up.Normalize();
}

void camera::set_center() {
	double wd2 = m_near_plane * tan(DTOR * m_fov / 2.0f);
	double left = - m_ratio * wd2;
	double right = m_ratio * wd2;
	double top = wd2;
	double bottom = -wd2;
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(left, right, bottom, top, m_near_plane, m_far_plane);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			m_pos.x, m_pos.y, m_pos.z, 
			m_target.x, m_target.y, m_target.z, 
			m_up.x, m_up.y, m_up.z);
}

void camera::set_left() {
	m_near_plane = (m_target - m_pos).Length() / 5.0f;
	vector3 eyepos = (m_up % (m_target - m_pos)).Normalize();
	eyepos *= m_eye_separation / 2.0f;
	double wd2 = m_near_plane * tan(DTOR * m_fov / 2.0f);
	double ndfl = m_near_plane / (m_target - m_pos).Length();
	double left = - m_ratio * wd2 + 0.5 * m_eye_separation * ndfl;
	double right = m_ratio * wd2 + 0.5 * m_eye_separation * ndfl;
	double top = wd2;
	double bottom = -wd2;
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(left, right, bottom, top, m_near_plane, m_far_plane);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			m_pos.x + eyepos.x, m_pos.y + eyepos.y, m_pos.z + eyepos.z, 
			m_target.x, m_target.y, m_target.z, 
			m_up.x, m_up.y, m_up.z);
}

void camera::set_right() {
	m_near_plane = (m_target - m_pos).Length() / 5.0f;
	vector3 eyepos = ((m_target - m_pos) % m_up).Normalize();
	eyepos *= m_eye_separation / 2.0f;
	double wd2 = m_near_plane * tan(DTOR * m_fov / 2.0f);
	double ndfl = m_near_plane / (m_target - m_pos).Length();
	double left = - m_ratio * wd2 - 0.5 * m_eye_separation * ndfl;
	double right = m_ratio * wd2 - 0.5 * m_eye_separation * ndfl;
	double top = wd2;
	double bottom = -wd2;
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(left, right, bottom, top, m_near_plane, m_far_plane);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			m_pos.x + eyepos.x, m_pos.y + eyepos.y, m_pos.z + eyepos.z, 
			m_target.x, m_target.y, m_target.z, 
			m_up.x, m_up.y, m_up.z);
}

vector3& camera::dir() {
	static vector3 dir = vector3(0.0f, 0.0f, 0.0f);
	dir = m_target - m_pos;
	dir.y = 0.0f;
	dir.Normalize();
	return dir;
}