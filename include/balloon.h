#pragma once

#include "object.h"
#include <allegro5/allegro_ttf.h>

class Balloon: public Object {
	static ALLEGRO_BITMAP *balloonMap;
	static ALLEGRO_FONT *font;

	std::vector<std::string> lines;
	Object *parent;
	int timer = 150; // 3 sec
public:
	Balloon(Room *r, Object *_parent, const std::string &text);

	void updatePositionToParent() {
		const int margin = 16;
		setLocation(parent->getx() - (w / 2) + (parent->getw() / 2), parent->gety() - h - margin);
	}

	void handleCollission(ObjectBase *o) override {}
	void draw(const GraphicsContext &gc) override;

	void update() override {
		if (!parent->isAlive() || --timer <= 0) { kill(); }
		else { updatePositionToParent(); }
	}

	static void init(std::shared_ptr<Resources> &res);
};
