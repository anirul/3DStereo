//////////////////////////////////////////////////////////////////////////
// block Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// block handler.

#ifndef __3DSTEREO_BLOCK_HEADER_DEFINED__
#define __3DSTEREO_BLOCK_HEADER_DEFINED__

enum block_type {
	// structural block
	BLOCK_VECTOR = 0,
	// debug block
	BLOCK_DEBUG = 10,
	// hit table
	BLOCK_HIT_WALKABLE = 11,
	BLOCK_HIT_DIE = 12,
	BLOCK_HIT_END = 13,
	// block with 3ds files
	BLOCK_BALCON = 20,
	BLOCK_COIN = 21,
	BLOCK_CROISEMENTPASSE = 22,
	BLOCK_COULOIRCROISEMENT = 23,
	BLOCK_COULOIRDROIT = 24,
	BLOCK_COULOIRDROITBORD = 25,
	BLOCK_COULOIRESCALIER = 26,
	BLOCK_COULOIRFIN = 27,
	BLOCK_COULOIRSALLE = 28,
	BLOCK_CUBE = 29,
	BLOCK_CUVE = 30,
	BLOCK_PASSERELLE1M = 31,
	BLOCK_PASSERELLE8M = 32,
	BLOCK_PASSERELLEBREAK = 33,
	BLOCK_PASSERELLEESCALIER = 34,
	BLOCK_SALLE = 35,
	BLOCK_SALLEMURE = 36,
	BLOCK_SALLEMURECOIN = 37,
	BLOCK_SALLEMURECOULOIR = 38,
	BLOCK_SALLEPASSERELLE = 39,
	BLOCK_SALLEPILIER = 40,
	BLOCK_TELEPORTEUR = 41,
	// new
	BLOCK_4PILIER = 42,
	BLOCK_CUBESKY = 43,
	BLOCK_PILIER = 44,
	BLOCK_SALLE_BASE_ELE = 45,
	BLOCK_SALLE_BORD_MURE = 46,
	BLOCK_SALLE_VIDE = 47,
	BLOCK_MURE = 48,
	BLOCK_MURE2 = 49,
	BLOCK_TOPSKY = 50,
	BLOCK_SOL = 51, 
	BLOCK_PASSERELLEJOINT = 52,
	BLOCK_ELE = 53,
	// end
	BLOCK_UNKNOWN = 60
};

const vector3 vnull(0.0f);
bool operator==(const vector3& v1, const vector3& v2);

class block_hit;

struct block_info {
	block_info(m3ds* mod, std::string name, std::string file) 
		: m_model(mod), m_name(name), m_file(file) {}
	block_info()
		: m_model(NULL), m_name(""), m_file("") {}
	~block_info();
	m3ds* m_model;
	std::string m_name;
	std::string m_file;
	std::vector<block_hit*> m_bhv;
};

class modellist : public std::map<block_type, block_info> {
private :
	static modellist* m_instance;
protected :
	modellist();
	modellist(const modellist& ml) {}
	virtual ~modellist() {}
public:
	static modellist* instance();
	static void load_block_list(std::string& filename);
	static void load_hit_list(std::string& filename);
	static void display(block_type bt);
	static const vector3& hit(block_type bt, const vector3& pos, const vector3& delta);
	static void release();
};

class block {
public :
	virtual bool loadfile(std::istream& file) = 0;
	virtual bool savefile(std::ostream& file) = 0;
	virtual void animate(float deltat) = 0;
	virtual const vector3& hit(const vector3& pos, const vector3& delta) = 0;
	virtual void display() = 0;
	virtual block_type get_type() = 0;
};

enum block_hit_type {
    BHT_WALKABLE = 11,
	BHT_DIE = 12,
	BHT_END = 13
};

class block_vector : public block, public std::vector<block*> {
	vector3 m_pos;
	vector3 m_scale;
	vector4 m_rot;
	matrix m_matdelta;
	matrix m_matinvdelta;
public :
	block_vector();
	~block_vector();
	virtual bool loadfile(std::istream& file);
	virtual bool savefile(std::ostream& file);
	virtual void animate(float deltat);
	virtual const vector3& hit(const vector3& pos, const vector3& delta);
	virtual void display();
	virtual block_type get_type() { return BLOCK_VECTOR; }
};

class block_debug : public block {
	vector3 m_v1;
	vector3 m_v2;
	vector3 m_pos;
	vector3 m_scale;
	vector4 m_rot;
public :
	block_debug(vector3& pos1, vector3& pos2);
	block_debug();
	virtual ~block_debug() {}
	virtual bool loadfile(std::istream& file);
	virtual bool savefile(std::ostream& file);
	virtual void animate(float deltat) {}
	virtual const vector3& hit(const vector3& pos, const vector3& delta);
	virtual void display();
	virtual block_type get_type() { return BLOCK_DEBUG;	}
};

class block_hit : public block {
	block_hit_type m_bht;
	vector3 m_pos1;
	vector3 m_pos2;
#ifdef _DEBUG
	block_debug* m_dbg;
#endif
	bool inside(vector3& pos);
public :
	block_hit();
	~block_hit();
	virtual bool loadfile(std::istream& file);
	virtual bool savefile(std::ostream& file);
	virtual void animate(float deltat) {}
	virtual const vector3& hit(const vector3& pos, const vector3& delta);
	virtual void display();
	virtual block_type get_type();
};

template<block_type t_bt>
class block_3ds : public block {
protected :
	vector3 m_pos;
	vector3 m_scale;
	vector4 m_rot;
	matrix m_matdelta;
	matrix m_matinvdelta;
public :
	block_3ds();
	virtual ~block_3ds();
	virtual bool loadfile(std::istream& file);
	virtual bool savefile(std::ostream& file);
	virtual void animate(float deltat);
	virtual const vector3& hit(const vector3& pos, const vector3& delta);
	virtual void display();
	virtual block_type get_type();
};

class block_balcon : public block_3ds<BLOCK_BALCON> {};
class block_coin : public block_3ds<BLOCK_COIN> {};
class block_croisementpasse : public block_3ds<BLOCK_CROISEMENTPASSE> {};
class block_couloircroisement : public block_3ds<BLOCK_COULOIRCROISEMENT> {};
class block_couloirdroit : public block_3ds<BLOCK_COULOIRDROIT> {};
class block_couloirdroitbord : public block_3ds<BLOCK_COULOIRDROITBORD> {};
class block_couloirescalier : public block_3ds<BLOCK_COULOIRESCALIER> {};
class block_couloirfin : public block_3ds<BLOCK_COULOIRFIN> {};
class block_couloirsalle : public block_3ds<BLOCK_COULOIRSALLE> {};
class block_cube : public block_3ds<BLOCK_CUBE> {};
class block_cuve : public block_3ds<BLOCK_CUVE> {};
class block_passerelle1m : public block_3ds<BLOCK_PASSERELLE1M> {};
class block_passerelle8m : public block_3ds<BLOCK_PASSERELLE8M> {};
class block_passerellebreak : public block_3ds<BLOCK_PASSERELLEBREAK> {};
class block_passerelleescalier : public block_3ds<BLOCK_PASSERELLEESCALIER> {};
class block_salle : public block_3ds<BLOCK_SALLE> {};
class block_sallemure : public block_3ds<BLOCK_SALLEMURE> {};
class block_sallemurecoin : public block_3ds<BLOCK_SALLEMURECOIN> {};
class block_sallemurecouloir : public block_3ds<BLOCK_SALLEMURECOULOIR> {};
class block_sallepasserelle : public block_3ds<BLOCK_SALLEPASSERELLE> {};
class block_sallepilier : public block_3ds<BLOCK_SALLEPILIER> {};
class block_teleporteur : public block_3ds<BLOCK_TELEPORTEUR> {};
class block_4pilier : public block_3ds<BLOCK_4PILIER> {};
class block_cubesky : public block_3ds<BLOCK_CUBESKY> {};
class block_pilier : public block_3ds<BLOCK_PILIER> {};
class block_salle_base_ele : public block_3ds<BLOCK_SALLE_BASE_ELE> {};
class block_salle_bord_mure : public block_3ds<BLOCK_SALLE_BORD_MURE> {};
class block_salle_vide : public block_3ds<BLOCK_SALLE_VIDE> {};
class block_mure : public block_3ds<BLOCK_MURE> {};
class block_mure2 : public block_3ds<BLOCK_MURE2> {};
class block_topsky : public block_3ds<BLOCK_TOPSKY> {};
class block_sol : public block_3ds<BLOCK_SOL> {};
class block_passerellejoint : public block_3ds<BLOCK_PASSERELLEJOINT> {};
class block_ele : public block_3ds<BLOCK_ELE> {};

std::istream& operator>>(std::istream& is, vector3& vec);
std::istream& operator>>(std::istream& is, vector4& vec);

std::ostream& operator<<(std::ostream& os, vector3& vec);
std::ostream& operator<<(std::ostream& os, vector4& vec);

matrix& rotation(vector4& rot);

template<block_type t_bt>
block_3ds<t_bt>::block_3ds() {	
	m_pos = vector3(0.0f);
	m_scale = vector3(1.0f);
	m_rot = vector4(0.0f);
	m_matdelta.IdentityMatrix();
	m_matinvdelta.IdentityMatrix();
	modellist* ml = modellist::instance();
	std::map<block_type, block_info>::iterator ite;
	if (!ml->operator [](get_type()).m_model) {
		ml->operator [](get_type()).m_model = 
			new m3ds(ml->operator[](get_type()).m_file);
	}
}

template<block_type t_bt>
block_3ds<t_bt>::~block_3ds() {}

template<block_type t_bt>
bool block_3ds<t_bt>::loadfile(std::istream& file) {
	std::string str = "";
	modellist* ml = modellist::instance();
	std::string error = "Parse error in " + 
		ml->operator[](get_type()).m_name + "::loadfile";
	file >> str;
	if (str != "{") 
		throw error + "missing {\n";
	file >> m_pos;
	file >> m_scale;
	file >> m_rot;
	file >> str;
	if (str != "}") 
		throw error + "missing }\n";

	// just for vector
	m_matdelta._11 = 1.0f / m_scale.x;
	m_matdelta._22 = 1.0f / m_scale.y;
	m_matdelta._33 = 1.0f / m_scale.z;
	m_matdelta *= rotation(m_rot);
	m_matinvdelta = m_matdelta;
	m_matinvdelta.Inverse();
	return true;
}

template<block_type t_bt>
bool block_3ds<t_bt>::savefile(std::ostream& file) {
	modellist* ml = modellist::instance();
	file << ml->operator[](get_type()).m_name 
		<< "{" << std::endl;
	file << m_pos;
	file << m_scale;
	file << m_rot;
	file << "}" << std::endl;
	return true;
}

template<block_type t_bt>
void block_3ds<t_bt>::animate(float deltat) {}

template<block_type t_bt>
const vector3& block_3ds<t_bt>::hit(const vector3& pos, const vector3& delta) {
	vector3 npos = pos;
	npos -= m_pos;
	npos = vector3((vector4(npos.vec) * m_matdelta).vec);
	vector3 ndelta = vector3((vector4(delta.vec) * m_matdelta).vec);
	static vector3 rdelta;
	rdelta = vnull;
	rdelta = vector3((vector4(modellist::instance()->hit(t_bt, npos, ndelta).vec) * m_matinvdelta).vec);
	return rdelta;
}

template<block_type t_bt>
void block_3ds<t_bt>::display() {
	modellist* ml = modellist::instance();
	glPushMatrix();
		glTranslatef(m_pos.x, m_pos.y, m_pos.z);
		glScalef(m_scale.x, m_scale.y, m_scale.z);
		glRotatef(m_rot.w, m_rot.x, m_rot.y, m_rot.z);
/*		glGetFloatv(GL_MODELVIEW_MATRIX, &m_matdelta._11);
		m_matdelta.Transpose();
		m_matinvdelta = m_matdelta;
		m_matinvdelta.Inverse(); */
		ml->display(t_bt);
	glPopMatrix();
}

template<block_type t_bt>
block_type block_3ds<t_bt>::get_type() {
	return t_bt;
}

#endif // __3DSTEREO_BLOCK_HEADER_DEFINED__