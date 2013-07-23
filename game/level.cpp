#include "main.h"

level::level(vector3& stpos, vector3& stdir, block_vector* rtbl) {
	m_name = "anonymous";
	m_stpos = stpos;
	m_stdir = stdir;
	m_root = rtbl;
}

level::level(std::string& filename) {
	m_stpos = vector3(0.0f, 0.0f, 0.0f);
	m_stdir = vector3(0.0f, 0.0f, -1.0f);
	m_root = NULL;
	m_name = filename;
	loadfile(filename);
}

level::~level() {
	if (m_root)
		delete m_root;
	m_root = NULL;
}

void level::animate(float deltat) {
	if (m_root)
		m_root->animate(deltat);
}

void level::display() {
	if (m_root)
		m_root->display();
}

bool level::loadfile(std::string& filename) {
	std::string excep = "Error in level::loadfile";
	std::ifstream ifs;
	ifs.open(filename.c_str());
	std::string str;
	ifs >> str;
	ifs >> m_stpos;
	ifs >> m_stdir;
	m_name = str;
	ifs >> str;
	if (str != "{") {
		ifs.close();
		throw excep + " missing {.\n";
	}
	m_root = new block_vector;
	ifs >> str;
	if (str != "block_vector") {
		ifs.close();
		throw excep + "missing block_vector description.\n";
	}
	m_root->loadfile(ifs);
	ifs >> str;
	if (str != "}") {
		ifs.close();
		throw excep + " missing }.\n";
	}
	ifs.close();
	return true;
}

bool level::savefile(std::string& filename) {
	std::ofstream ofs;
	ofs.open(filename.c_str());
	ofs	<< m_name
		<< " "
		<< m_stpos
		<< " "
		<< m_stdir
		<< " "
		<< "{"
		<< std::endl;
	m_root->savefile(ofs);
	ofs << "}" << std::endl;
	ofs.close();
	return true;
}

const vector3& level::hit(const vector3& pos, const vector3& delta) {
	if (m_root)
		return m_root->hit(pos, delta);
	else 
		return delta;
}