#include "irisshader.h"

#include <allegro5/allegro.h>
#include "mainloop.h"
#include "resources.h"

using namespace std;

ALLEGRO_BITMAP *IrisEffect::gradient;

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

void IrisEffect::enable(float time, int ofstx, int ofsty) {
	al_use_shader(shader);

	al_set_shader_sampler("gradientMap", gradient, 1);
	int offset[2] = {ofstx, ofsty };

	al_set_shader_int("ofstx", ofstx);
	al_set_shader_int("ofsty", ofsty);
	al_set_shader_float("time",time);
}

void IrisEffect::disable() {
	al_use_shader(nullptr);
}

void IrisEffect::init(std::shared_ptr<Resources> resources) {
	fragShaderSource = resources->getTextFile("iris_frag");
	gradient = resources->getBitmap("gradient");
	inited = true;
}

string IrisEffect::fragShaderSource;
string IrisEffect::vertShaderSource;
bool IrisEffect::inited = false;
