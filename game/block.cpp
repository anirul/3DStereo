#include "main.h"

bool operator==(const vector3& v1, const vector3& v2) {
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) return true;
	else return false;
}

bool operator!=(const vector3& v1, const vector3& v2) {
	if (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z) return true;
	else return false;
}

std::istream& operator>>(std::istream& is, vector3& vec) {
	std::string str = "";
	std::string error = "Parse error in vector3::operator>>";
	is >> str;
	if (str != "{")
		throw error + " missing {.\n";
	is >> vec.x;
	is >> vec.y;
	is >> vec.z;
	is >> str;
	if (str != "}")
		throw error + " missing }.\n";
	return is;
}

std::istream& operator>>(std::istream& is, vector4& vec) {
	std::string str = "";
	std::string error = "Parse error in vector4::operator>>";
	is >> str;
	if (str != "{")
		throw error + " missing {.\n";
	is >> vec.x;
	is >> vec.y;
	is >> vec.z;
	is >> vec.w;
	is >> str;
	if (str != "}")
		throw error + " missing }.\n";
	return is;
}

std::ostream& operator<<(std::ostream& os, vector3& vec) {
	os	<< "{ " 
		<< vec.x 
		<< " " 
		<< vec.y 
		<< " " 
		<< vec.z 
		<< " }" 
		<< std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, vector4& vec) {
	os	<< "{ " 
		<< vec.x 
		<< " " 
		<< vec.y 
		<< " " 
		<< vec.z 
		<< " " 
		<< vec.w 
		<< " }" 
		<< std::endl;
	return os;
}

matrix& rotation(vector4& rot) {
	static matrix m;
	float c = cos(DTOR * -rot.w);
	float s = sin(DTOR * -rot.w);
	float t = 1 - cos(DTOR * -rot.w);
	float x = rot.x;
	float y = rot.y;
	float z = rot.z;
	matrix temp(
		t * x * x + c,		t * y * x + s * z,	t * x * z - s * y,	0,
		t * x * y - s * z,	t * y * y + c,		t * y * z + s * x,	0,
		t * x * z + s * y,	t * y * z - s * z,	t * z * z + c,		0,
		0,					0,					0,					1);
	m = temp;
	return m;
}

modellist* modellist::m_instance = 0;

block_info::~block_info() {
	if (m_model)
		delete m_model;
}

modellist::modellist() {}

modellist* modellist::instance() {
	if (!m_instance) {
		m_instance = new modellist;
		modellist::load_block_list(std::string("block.bin"));
		modellist::load_hit_list(std::string("hit.bin"));
	}
	return m_instance;
}

void modellist::load_block_list(std::string& filename) {
	if (!m_instance) return;
	std::ifstream ifs;
	ifs.open(filename.c_str());
	int n = 0;
	std::string name;
	std::string file;
	do {
		n = 0;
		name.clear();
		file.clear();
		ifs >> (int)n;
		ifs >> name;
		ifs >> file;
		if (ifs.eof()) break;
		m_instance->insert(modellist::value_type(
			(block_type)n, block_info(NULL, name, file)));
	} while (!ifs.eof());
	ifs.close();
}

void modellist::load_hit_list(std::string& filename) {
	if (!m_instance) return;
	std::ifstream ifs;
	ifs.open(filename.c_str());
	int block_number = 0;
	int block_hit_count = 0;
	do {
		block_number = 0;
		ifs >> (int)block_number;
		ifs >> (int)block_hit_count;
		std::map<block_type, block_info>::iterator ite;
		for (int i = 0; i < block_hit_count; ++i) {
			block_hit* pbh = new block_hit();
			pbh->loadfile(ifs);
			ite = m_instance->find((block_type)block_number);
			if (ite != m_instance->end())
				ite->second.m_bhv.push_back(pbh);
		}
	} while (!ifs.eof());
	ifs.close();
}

void modellist::display(block_type bt) {
	if (!m_instance) return;
	std::map<block_type, block_info>::iterator ite;
	ite = m_instance->find(bt);
	std::vector<block_hit*>::iterator ite2;
	for (ite2 = ite->second.m_bhv.begin(); ite2 != ite->second.m_bhv.end(); ite2++)
		(*ite2)->display();
	ite->second.m_model->display();
}

const vector3& modellist::hit(block_type bt, const vector3& pos, const vector3& delta) {
	if (!m_instance) return vnull;
	std::map<block_type, block_info>::iterator ite;
	ite = m_instance->find(bt);
	std::vector<block_hit*>::iterator ite2;
	static vector3 ndelta;
	ndelta = vnull;
	vector3 v = vnull;
	for (ite2 = ite->second.m_bhv.begin(); ite2 != ite->second.m_bhv.end(); ite2++)
		if ((v = (*ite2)->hit(pos, delta)) != vnull)
			if (v.Length() > ndelta.Length()) 
				ndelta = v;
	return ndelta;
}


void modellist::release() {
	if (m_instance) {
		std::map<block_type, block_info>::iterator ite;
		for (ite = m_instance->begin(); ite != m_instance->end(); ite++) {
			std::vector<block_hit*>::iterator ite2;
			for (ite2 = ite->second.m_bhv.begin(); ite2 != ite->second.m_bhv.end(); ite2++) {
				block_hit* pbh = (*ite2);
				if (pbh) delete pbh;
			}
			ite->second.m_bhv.clear();
		}
		m_instance->clear();
		delete m_instance;
		m_instance = 0;
	}
}

block_hit::block_hit() {
	m_bht = BHT_WALKABLE;
	m_pos1 = vector3(0.0f);
	m_pos2 = vector3(0.0f);
#ifdef _DEBUG
	m_dbg = 0;
#endif
}

block_hit::~block_hit() {
#ifdef _DEBUG
	if (m_dbg) delete m_dbg;
#endif
}

bool block_hit::loadfile(std::istream& file) {
	std::string str = "";
	std::string error = "Parse error in block_hit::loadfile";
	int n = 0;
	file >> str;
	if (str != "{")
		throw error + "missing {\n";
	file >> (int)n;
	if (n < 11 || n > 13) 
		throw error + "illegal block_hit_type\n";
	m_bht = (block_hit_type)n;
	file >> m_pos1;
	file >> m_pos2;
#ifdef _DEBUG
	m_dbg = new block_debug(m_pos1, m_pos2);
#endif
	file >> str;
	if (str != "}") 
		throw error + "missing }.\n";
	return true;
}

bool block_hit::savefile(std::ostream& file) {
	file << "block_hit " << " { " << std::endl;
	file << (int)m_bht << std::endl;
	file << m_pos1;
	file << m_pos2;
	file << " }" << std::endl;
	return true;
}

bool block_hit::inside(vector3& pos) {
	if (pos.x > ((m_pos1.x > m_pos2.x) ? m_pos1.x : m_pos2.x)) return false;
	if (pos.x < ((m_pos1.x < m_pos2.x) ? m_pos1.x : m_pos2.x)) return false;
	if (pos.y > ((m_pos1.y > m_pos2.y) ? m_pos1.y : m_pos2.y)) return false;
	if (pos.y < ((m_pos1.y < m_pos2.y) ? m_pos1.y : m_pos2.y)) return false;
	if (pos.z > ((m_pos1.z > m_pos2.z) ? m_pos1.z : m_pos2.z)) return false;
	if (pos.z < ((m_pos1.z < m_pos2.z) ? m_pos1.z : m_pos2.z)) return false;
	if ((m_bht == BHT_END) || (m_bht == BHT_DIE))
		throw m_bht;
	return true;
}

const vector3& block_hit::hit(const vector3& pos, const vector3& delta) {
	static vector3 ndelta;
	ndelta = delta;
	if (inside(pos + delta)) return delta;
	ndelta.z = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = delta;
	ndelta.x = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = delta;
	ndelta.y = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = delta;
	ndelta.x = 0.0f;
	ndelta.y = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = delta;
	ndelta.x = 0.0f;
	ndelta.z = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = delta;
	ndelta.z = 0.0f;
	ndelta.y = 0.0f;
	if (inside(pos + ndelta)) return ndelta;
	ndelta = vector3(0.0f);
	return ndelta;
}

void block_hit::display() {
#ifdef _DEBUG
	if (m_dbg) m_dbg->display();
#endif
}

block_type block_hit::get_type() {
	switch(m_bht) {
		case BHT_WALKABLE :
			return BLOCK_HIT_WALKABLE;
		case BHT_DIE :
			return BLOCK_HIT_DIE;
		case BHT_END :
			return BLOCK_HIT_END;
		default :
			return BLOCK_UNKNOWN;
	}
}

block_vector::block_vector() {
	m_pos = vector3(0.0f);
	m_scale = vector3(1.0f);
	m_rot = vector4(0.0f);
	m_matdelta.IdentityMatrix();
	m_matinvdelta.IdentityMatrix();
	this->clear();
}

block_vector::~block_vector() {
	std::vector<block*>::iterator ite;
	for (ite = this->begin(); ite != this->end(); ite++) {
		if (*ite) delete (*ite);
	}
	this->clear();
}

#define CI(x) (str == #x) { x* p = new x(); p->loadfile(file); push_back(p); }

bool block_vector::loadfile(std::istream& file) {
	std::string str = "";
	std::string error = "Parse error in block_vector::loadfile";
	int n = 0;
	file >> (int)n;
	file >> str;
	if (str != "{")
		throw error + "missing {\n";
	file >> m_pos;
	file >> m_scale;
	file >> m_rot;
	for (int i = 0; i < n; ++i) {
		file >> str;
		if      CI(block_vector) 
		else if CI(block_balcon)
		else if CI(block_coin)
		else if CI(block_croisementpasse)
		else if CI(block_couloircroisement)
		else if CI(block_couloirdroit)
		else if CI(block_couloirdroitbord)
		else if CI(block_couloirescalier)
		else if CI(block_couloirfin)
		else if CI(block_couloirsalle)
		else if CI(block_cube)
		else if CI(block_cuve)
		else if CI(block_passerelle1m)
		else if CI(block_passerelle8m)
		else if CI(block_passerellebreak)
		else if CI(block_passerelleescalier)
		else if CI(block_salle)
		else if CI(block_sallemure)
		else if CI(block_sallemurecoin)
		else if CI(block_sallemurecouloir)
		else if CI(block_sallepasserelle)
		else if CI(block_sallepilier)
		else if CI(block_teleporteur)
		else if CI(block_debug)
		else if CI(block_hit)
		else if CI(block_4pilier)
		else if CI(block_cubesky)
		else if CI(block_pilier)
		else if CI(block_salle_base_ele)
		else if CI(block_salle_bord_mure)
		else if CI(block_salle_vide)
		else if CI(block_mure)
		else if CI(block_mure2)
		else if CI(block_topsky)
		else if CI(block_sol)
		else if CI(block_passerellejoint)
		else if CI(block_ele)
		else throw error + "unknown block " + str + ".\n";
	}
	file >> str;
	if (str != "}") 
		throw error + "missing }.\n";

	// just for vector
	m_matdelta._11 = 1.0f / m_scale.x;
	m_matdelta._22 = 1.0f / m_scale.y;
	m_matdelta._33 = 1.0f / m_scale.z;
	m_matdelta *= rotation(m_rot);
	m_matinvdelta = m_matdelta;
	m_matinvdelta.Inverse();

	return true;
}

#undef CI

bool block_vector::savefile(std::ostream& file) {
	file << "block_vector " << (unsigned int)this->size() << " {" << std::endl;
	file << m_pos;
	file << m_scale;
	file << m_rot;
	std::vector<block*>::iterator ite;
	for (ite = this->begin(); ite != this->end(); ite++)
		(*ite)->savefile(file);
	file << "}" << std::endl;
	return true;
}

void block_vector::animate(float deltat) {
	std::vector<block*>::iterator ite;
	for (ite = this->begin(); ite != this->end(); ite++)
		(*ite)->animate(deltat);
}

const vector3& block_vector::hit(const vector3& pos, const vector3& delta) {
	vector3 npos = pos;
	npos -= m_pos;
	npos = vector3((vector4(npos.vec) * m_matdelta).vec);
	vector3 ndelta = vector3((vector4(delta.vec) * m_matdelta).vec);
	vector3 tdelta = vnull;
	static vector3 rdelta;
	rdelta = vnull;
	std::vector<block*>::iterator ite;
	for (ite = this->begin(); ite != this->end(); ite++) {
		tdelta = vector3((vector4((*ite)->hit(npos, ndelta).vec) * m_matinvdelta).vec);
		if (tdelta.Length() > rdelta.Length())
			rdelta = tdelta;
	}
	return rdelta;
}

void block_vector::display() {
	glPushMatrix();
		glTranslatef(m_pos.x, m_pos.y, m_pos.z);
		glScalef(m_scale.x, m_scale.y, m_scale.z);
		glRotatef(m_rot.w, m_rot.x, m_rot.y, m_rot.z);
		std::vector<block*>::iterator ite;
		for (ite = this->begin(); ite != this->end(); ite++)
			(*ite)->display();
	glPopMatrix();
}

block_debug::block_debug() {
	m_v1 = vector3(0.0f);
	m_v2 = vector3(1.0f);
	m_pos = vector3(0.0f);
	m_scale = vector3(1.0f);
	m_rot = vector4(0.0f);
}

block_debug::block_debug(vector3& pos1, vector3& pos2) {
	m_v1 = pos1;
	m_v2 = pos2;
	m_pos = vector3(0.0f);
	m_scale = vector3(1.0f);
	m_rot = vector4(0.0f);
}

bool block_debug::loadfile(std::istream& file) {
	std::string str = "";
	modellist* ml = modellist::instance();
	std::string error = "Parse error in block_debug::loadfile";
	file >> str;
	if (str != "{") 
		throw error + "missing {\n";
	file >> m_v1;
	file >> m_v2;
	file >> m_pos;
	file >> m_scale;
	file >> m_rot;
	file >> str;
	if (str != "}") 
		throw error + "missing }\n";
	return true;
}

bool block_debug::savefile(std::ostream& file) {
	file << "block_debug"
		<< " { " << std::endl;
	file << m_v1;
	file << m_v2;
	file << m_pos;
	file << m_scale;
	file << m_rot;
	file << " } " << std::endl;
	return true;

}

const vector3& block_debug::hit(const vector3& pos, const vector3& delta) {
	return vnull;
}

void block_debug::display() {
	glPushMatrix();
		glTranslatef(m_pos.x, m_pos.y, m_pos.z);
		glScalef(m_scale.x, m_scale.y, m_scale.z);
		glRotatef(m_rot.w, m_rot.x, m_rot.y, m_rot.z);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
			glVertex3fv(m_v1.vec.m128_f32);
			glVertex3f(m_v2.x, m_v1.y, m_v1.z);
			glVertex3f(m_v2.x, m_v1.y, m_v2.z);
			glVertex3fv(m_v2.vec.m128_f32);
			glVertex3f(m_v2.x, m_v2.y, m_v1.z);
			glVertex3f(m_v1.x, m_v2.y, m_v1.z);
			glVertex3f(m_v1.x, m_v2.y, m_v2.z);
			glVertex3f(m_v1.x, m_v1.y, m_v2.z);
		glEnd();
		glBegin(GL_LINES);
			glVertex3fv(m_v1.vec.m128_f32);
			glVertex3f(m_v1.x, m_v2.y, m_v1.z);
			glVertex3f(m_v2.x, m_v1.y, m_v1.z);
			glVertex3f(m_v2.x, m_v2.y, m_v1.z);
			glVertex3fv(m_v2.vec.m128_f32);
			glVertex3f(m_v1.x, m_v2.y, m_v2.z);
			glVertex3f(m_v1.x, m_v1.y, m_v2.z);
			glVertex3f(m_v2.x, m_v1.y, m_v2.z);
		glEnd();
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
			glVertex3f(m_v1.x - 1.0f, m_v1.y, m_v1.z);
			glVertex3f(m_v1.x + 1.0f, m_v1.y, m_v1.z);
			glVertex3f(m_v1.x, m_v1.y - 1.0f, m_v1.z);
			glVertex3f(m_v1.x, m_v1.y + 1.0f, m_v1.z);
			glVertex3f(m_v1.x, m_v1.y, m_v1.z - 1.0f);
			glVertex3f(m_v1.x, m_v1.y, m_v1.z + 1.0f);
		glEnd();
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}
