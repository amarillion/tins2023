#include "balloon.h"
#include "frame.h"
#include "color.h"
#include <allegro5/allegro_ttf.h>

ALLEGRO_BITMAP *Balloon::balloonMap = nullptr;
ALLEGRO_FONT *Balloon::font = nullptr;

const Point BALLOON_TILE_SIZE { 8, 8 };

void Balloon::draw(const GraphicsContext &gc) {
	Point base { (int)x - gc.xofst, (int)y - gc.yofst };

	Rect destRect { base.x(), base.y(), w, h };
	drawFrame(balloonMap, destRect, BALLOON_TILE_SIZE, bubbleMapFunc);

	al_draw_text(font, BLACK, base.x() + (w / 2), base.y() + ((h - al_get_font_line_height(font)) / 2), ALLEGRO_ALIGN_CENTER, text.c_str());
}

void Balloon::init(std::shared_ptr<Resources> &res) {
	balloonMap = res->getBitmap("bubble1");
	font = res->getFont("builtin_font")->get();
}
