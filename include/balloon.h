#pragma once

#include "object.h"
#include <allegro5/allegro_ttf.h>

class Balloon: public Object {
	static ALLEGRO_BITMAP *balloonMap;
	static ALLEGRO_FONT *font;

	std::string text;
public:
	Balloon(Room *r, int type, std::string _text): Object(r, type), text(std::move(_text)) {
		w = al_get_text_width(font, text.c_str()) + 16;
		h = 24;
		solid = false;
	}

	void handleCollission(ObjectBase *o) override {};
	void draw(const GraphicsContext &gc) override;

	static void init(std::shared_ptr<Resources> &res);
};
