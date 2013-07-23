#include "main.h"

static int s_count = 0;
texlib* texlib::m_instance = 0;
static const char* s_prefix = "models\\3ds\\";

texlib* texlib::instance() {
	if (!m_instance)
		m_instance = new texlib;
	return m_instance;
}

void texlib::release() {
	if (m_instance) {
		std::map<std::string, texture*>::iterator ite;
		for (ite = m_instance->begin(); ite != m_instance->end(); ite++)
			if (ite->second)
				delete (ite->second);
		m_instance->clear();
		delete m_instance;
		m_instance = 0;
	}
}

m3ds::m3ds() {
	m_filename = "";
	m_file = NULL;
	m_current_frame = 0.0f;
	s_count++;
}

m3ds::m3ds(std::string& filename) {
	m_filename = "";
	m_file = NULL;
	m_current_frame = 0.0f;
	s_count++;
	load(filename);
}

m3ds::~m3ds() {
	if (m_file)
		lib3ds_file_free(m_file);
	s_count--;
	// unload textures
	if (s_count == 0) {
		texlib::release();
	}
}

bool m3ds::load(std::string& filename) {
	m_filename = filename;
	m_file = lib3ds_file_load(filename.c_str());
	if (!m_file)
		return false;
	lib3ds_file_eval(m_file, 0.0f);
	std::map<std::string, texture*>::iterator ite;
	ite = texlib::instance()->find("default.tga");
	if (ite == texlib::instance()->end()) {
		tga_tex* tex = new tga_tex("models\\3ds\\default.tga");
		texlib::instance()->insert(
			texlib::value_type(std::string("default.tga"), tex));
	}
	return true;
}

void m3ds::render_node(Lib3dsNode* node) {
	assert(m_file);
	{
		Lib3dsNode* p;
		for (p = node->childs; p != 0; p = p->next)
			render_node(p);
	}
	if (node->type == LIB3DS_OBJECT_NODE) {
		if (!strcmp(node->name, "$$$DUMMY")) return;
		if (!node->user.d) {
			Lib3dsMesh* mesh = lib3ds_file_mesh_by_name(m_file, node->name);
			assert(mesh);
			if (!mesh)
				return;
			node->user.d = glGenLists(1);
			glNewList(node->user.d, GL_COMPILE);
			{
				unsigned int p;
				Lib3dsVector* normalL = new Lib3dsVector[3 * mesh->faces];
				{
					Lib3dsMatrix M;
					lib3ds_matrix_copy(M, mesh->matrix);
					lib3ds_matrix_inv(M);
					glMultMatrixf(&M[0][0]);
				}
				lib3ds_mesh_calculate_normals(mesh, normalL);

				for (p = 0; p < mesh->faces; ++p) {
					Lib3dsFace* f = &mesh->faceL[p];
					Lib3dsMaterial* mat = 0;
					if (f->material[0]) {
						mat = lib3ds_file_material_by_name(m_file, f->material);
						if (mat->texture1_map.name[0]) {
							std::string str = mat->texture1_map.name;
							std::map<std::string, texture*>::iterator ite;
							ite = texlib::instance()->find(str);
							if (ite == texlib::instance()->end()) {
								texture* pt = NULL;
								char str2[512];
								memset(str2, 0, 512);
								sprintf(str2, "%s%s", s_prefix, str.c_str());
								char* ext = &str2[strlen(str2) - 3];
								ext[0] = tolower(ext[0]);
								ext[1] = tolower(ext[1]);
								ext[2] = tolower(ext[2]);
								if (!strcmp(ext, "tga")) {
									tga_tex* tex = new tga_tex(str2);
									if (tex->get_tex_id() != -1) {
										pt = tex;
										texlib::instance()->insert(texlib::value_type(str, pt));
									}
								} else if (!strcmp(ext, "bmp")) {
									bmp_tex* tex = new bmp_tex(str2);
									if (tex->get_tex_id() != -1) {
										pt = tex;
										texlib::instance()->insert(texlib::value_type(str, pt));
									}
								} else if (!strcmp(ext, "pcx")) {
									pcx_tex* tex = new pcx_tex(str2);
									if (tex->get_tex_id() != -1) {
										pt = tex;
										texlib::instance()->insert(texlib::value_type(str, pt));
									}
								} else {
									ext[0] = 't';
									ext[1] = 'g';
									ext[2] = 'a';
									tga_tex* tex = new tga_tex(str2);
//									str = str2;
									if (tex->get_tex_id() != -1) {
										pt = tex;
									} else {
										pt = texlib::instance()->operator[]("default.tga");
									}
									texlib::instance()->insert(texlib::value_type(str, pt));
								}
								glBindTexture(GL_TEXTURE_2D, (pt) ? pt->get_tex_id() :
									((*texlib::instance())["default.tga"])->get_tex_id());
							} else {
								glBindTexture(GL_TEXTURE_2D, (ite->second)->get_tex_id());
							}
						}	
					}
					if (mat) {
						static GLfloat a[4] = {0.0f, 0.0f, 0.0f, 1.0f};
						float s;
						glMaterialfv(GL_FRONT, GL_AMBIENT, a);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
						s = pow(2.0f, 10.0f * mat->shininess);
						if (s > 128.0) s = 128.0;
						glMaterialf(GL_FRONT, GL_SHININESS, s);
					} else {
						Lib3dsRgba a = {0.2f, 0.2f, 0.2f, 1.0f};
						Lib3dsRgba d = {0.8f, 0.8f, 0.8f, 1.0f};
						Lib3dsRgba s = {0.0f, 0.0f, 0.0f, 1.0f};
						glMaterialfv(GL_FRONT, GL_AMBIENT, a);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
						glMaterialfv(GL_FRONT, GL_SPECULAR, s);
					}
					glBegin(GL_TRIANGLES);
						glNormal3fv(f->normal);
						for (int i = 0; i < 3; ++i) {
							assert(mesh);
							if (mesh->texelL)
								glTexCoord2fv(mesh->texelL[f->points[i]]);
							glNormal3fv(normalL[3 * p + i]);
							glVertex3fv(mesh->pointL[f->points[i]].pos);
						}
					glEnd();
				}
				delete [] normalL;
			}
			glEndList();
		}
		if (node->user.d) {
			Lib3dsObjectData* d;
			glPushMatrix();
			d = &node->data.object;
			glMultMatrixf(&node->matrix[0][0]);
			glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
			glCallList(node->user.d);
			glPopMatrix();
		}
	}
}

void m3ds::display() {
	if (m_file) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		{
			Lib3dsNode* p;
			for (p = m_file->nodes; p != 0; p = p->next)
				render_node(p);
		}
	}
}