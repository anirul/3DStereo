//////////////////////////////////////////////////////////////////////////
// Level Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// Level handler.

#ifndef __3DSTEREO_LEVEL_HEADER_DEFINED__
#define __3DSTEREO_LEVEL_HEADER_DEFINED__

class level {
	std::string m_name;
	block* m_root;
	vector3 m_stpos;
	vector3 m_stdir;
public :
	level(vector3& stpos, vector3& stdir, block_vector* rtbl);
	level(std::string& filename);
	~level();
	std::string& name() { return m_name; }
	const std::string& name() const { return m_name; }
	bool loadfile(std::string& filename);
	bool savefile(std::string& filename);
	const vector3& hit(const vector3& pos, const vector3& delta);
	void animate(float deltat);
	void display();
};

#endif // __3DSTEREO_LEVEL_HEADER_DEFINED__