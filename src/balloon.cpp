#include "balloon.h"
#include "frame.h"
#include "color.h"
#include <allegro5/allegro_ttf.h>
#include "strutil.h"

ALLEGRO_BITMAP *Balloon::balloonMap = nullptr;
ALLEGRO_FONT *Balloon::font = nullptr;

const Point BALLOON_TILE_SIZE { 8, 8 };

void Balloon::draw(const GraphicsContext &gc) {
	Point base { (int)x - gc.xofst, (int)y - gc.yofst };

	Rect destRect { base.x(), base.y(), w, h };
	drawFrame(balloonMap, destRect, BALLOON_TILE_SIZE, bubbleMapFunc);

	int lineh = al_get_font_line_height(font);
	int yco = base.y() + ((h - lineh * lines.size()) / 2);
	for (const auto &line: lines) {
		al_draw_text(font, BLACK, base.x() + (w / 2), yco, ALLEGRO_ALIGN_CENTER, line.c_str());
		yco += lineh;
	}
}

void Balloon::init(std::shared_ptr<Resources> &res) {
	balloonMap = res->getBitmap("bubble1");
	font = res->getFont("builtin_font")->get();
}

Balloon::Balloon(Room *r, Object *_parent, const std::string &text) : Object(r, OT_NO_COLLISION), parent(_parent) {
	lines = split(text, '\n');
	int maxW = 0;
	for (const auto &line: lines) {
		int w = al_get_text_width(font, line.c_str());
		if (w > maxW) { maxW = w; }
	}
	w = maxW + 16;
	h = 16 + al_get_font_line_height(font) * lines.size();
	solid = false;
}