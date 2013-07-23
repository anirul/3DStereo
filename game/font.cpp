#include "main.h"

font::font(std::string& fontname) {
	m_fontname = fontname;
	m_r = 1.0f;
	m_g = 1.0f;
	m_b = 1.0f;
	m_fontlist[std::string("GLUT_BITMAP_9_BY_15")] = GLUT_BITMAP_9_BY_15;
	m_fontlist[std::string("GLUT_BITMAP_8_BY_13")] = GLUT_BITMAP_8_BY_13;
	m_fontlist[std::string("GLUT_BITMAP_TIMES_ROMAN_10")] = GLUT_BITMAP_TIMES_ROMAN_10;
	m_fontlist[std::string("GLUT_BITMAP_TIMES_ROMAN_24")] = GLUT_BITMAP_TIMES_ROMAN_24;
	m_fontlist[std::string("GLUT_BITMAP_HELVETICA_10")] = GLUT_BITMAP_HELVETICA_10;
	m_fontlist[std::string("GLUT_BITMAP_HELVETICA_12")] = GLUT_BITMAP_HELVETICA_12;
	m_fontlist[std::string("GLUT_BITMAP_HELVETICA_18")] = GLUT_BITMAP_HELVETICA_18;
}

font::~font() {}

void font::clear() {
	m_text.clear();
}

void font::clearlast() {
	m_text.pop_back();
}

void font::print(std::string& str) {
	m_text.push_back(str);
}

void font::color(float r, float g, float b) {
	m_r = r;
	m_g = g;
	m_b = b;
}

void font::display(float w, float h) {
	float x = -65.0f;
	float y = 50.0f;
	float dy = 2.0f;
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	glPushMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_TEXTURE_2D);
		glColor4f(m_r, m_g, m_b, 0.0);
		std::vector<std::string>::iterator ite;
		for (ite = m_text.begin(); ite != m_text.end(); ite++) {
			const char* str = (*ite).c_str();
			glRasterPos2f(x, y);
			if (str && strlen(str)) {
				while (*str) {
					glutBitmapCharacter(m_fontlist[m_fontname], *str);
					str++;
				}
			}
			y -= dy;
		}
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}