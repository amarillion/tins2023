#include "irisshader.h"

#include <allegro5/allegro.h>
#include "mainloop.h"
#include "resources.h"

using namespace std;

IrisEffect::IrisEffect()
{
	assert(inited);

	shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	assert(shader);

	bool ok;

	ok = al_attach_shader_source(shader, ALLEGRO_PIXEL_SHADER, fragShaderSource.c_str());
	//TODO: assert with message format...
	if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
	assert(ok);

	ok = al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER,
								 al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER));
//	ok = al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, vertShaderSource.c_str());

	//TODO: assert with message format...
	if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
	assert(ok);

	ok = al_build_shader(shader);
	//TODO: assert with message...
	if (!ok) printf ("al_build_shader failed: %s\n", al_get_shader_log(shader));
	assert(ok);
}

void IrisEffect::enable(float sigma, ALLEGRO_BITMAP *bmp) {
	al_use_shader(shader);

	al_set_shader_sampler("textureMap", bmp, 1);
	int texSize[2] = {
			al_get_bitmap_width(bmp),
			al_get_bitmap_height(bmp)
	};
//	al_set_shader_int_vector("u_texture_size", 2, texSize, 1);
	al_set_shader_float("time", sigma);
//	int dir[2] = { 0, 1 };
//	al_set_shader_int_vector("u_dir", 2, dir, 1);
}

void IrisEffect::disable() {
	al_use_shader(NULL);
}

void IrisEffect::init(std::shared_ptr<Resources> resources) {
	fragShaderSource = resources->getTextFile("iris_frag");
	vertShaderSource = resources->getTextFile("iris_vert");
	inited = true;
}

string IrisEffect::fragShaderSource;
string IrisEffect::vertShaderSource;
bool IrisEffect::inited = false;
