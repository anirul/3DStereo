//////////////////////////////////////////////////////////////////////////
// m3ds Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// model loader for 3DS files (use the lib3ds library).

#ifndef __3DSTEREO_M3DS_HEADER_DEFINED__
#define __3DSTEREO_M3DS_HEADER_DEFINED__

class texlib : public std::map<std::string, texture*> {
private :
	static texlib* m_instance;
protected :
	texlib() {}
	texlib(const texlib& ml) {}
	virtual ~texlib() {}
public:
	static texlib* instance();
	static void release();
};

class m3ds {
	float m_current_frame;
	std::string m_filename;
	Lib3dsFile* m_file;
	void render_node(Lib3dsNode* node);
public :
	m3ds(std::string& filename);
	m3ds();
	~m3ds();
	bool load(std::string& filename);
	const std::string& name() const { return m_filename; }
    void display();
};

#endif // __3DSTEREO_M3DS_HEADER_DEFINED__
