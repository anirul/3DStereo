//////////////////////////////////////////////////////////////////////////
// font Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// font and text handler class (using GLUT).
// support for ANSI color enabled!

#ifndef __3DSTEREO_FONT_HEADER_DEFINED__
#define __3DSTEREO_FONT_HEADER_DEFINED__

class font {
	std::vector<std::string> m_text;
	std::map<std::string, void*> m_fontlist;
	std::string m_fontname;
	float m_r;
	float m_g;
	float m_b;
public :
	font(std::string& fontname);
	~font();
	void display(float w = 0.0f, float h = 0.0f);
	void clear();
	void clearlast();
	void print(std::string& str);
	void color(float r, float g, float b);
};

#endif // __3DSTEREO_FONT_HEADER_DEFINED__