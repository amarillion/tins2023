#include "chartview.h"
#include "rect.h"
#include "frame.h"
#include "level.h"
#include "player.h"
#include "game.h"
#include "door.h"

void ChartView::draw(const GraphicsContext &gc) {
	const int basex = 8;
	const int basey = 8;

	bool mapCollected = game->isMapCollected();

	if (mapCollected) {
		Rect chartRect{0, 0, 128, 96};
		drawFrame(frame, chartRect, Point{8, 8}, chartMapFunc);
	}

	int chartw = al_get_bitmap_width(chart->getBitmap());
	int charth = al_get_bitmap_height(chart->getBitmap());

	al_draw_scaled_bitmap(chart->getBitmap(),
						  0, 0, chartw, charth,
						  basex, basey, chartw, charth / 2, 0);

	for (Object *o :game->getObjects()->getChildren()) {
		int xco = basex + o->getRoom()->mx * ROOM_WIDTH + (o->getx() / TILE_SIZE) - 2;
		int yco = basey + (o->getRoom()->my * ROOM_HEIGHT + (o->gety() / TILE_SIZE)) / 2 - 2;
		switch(o->getType()) {
			case OT_RESCUEE:
				if (mapCollected) al_draw_bitmap_region(mapIcons, 24, 0, 5, 5, xco, yco, 0);
				break;
			case OT_KEY:
				if (mapCollected) al_draw_bitmap_region(mapIcons, 16, 0, 5, 5, xco, yco, 0);
				break;
			case OT_TELEPORT:
				if (mapCollected) al_draw_bitmap_region(mapIcons, 32, 0, 5, 5, xco, yco, 0);
				break;
			case OT_SHOPKEEP:
				if (mapCollected) al_draw_bitmap_region(mapIcons, 48, 0, 5, 5, xco, yco, 0);
				break;
			case OT_LOCKED_DOOR:
				if (mapCollected) al_draw_bitmap_region(mapIcons, 40, 0, 5, 5, xco, yco, 0);
				break;
			case OT_PLAYER: {
				Player *player = dynamic_cast<Player *>(o);
				al_draw_bitmap_region(mapIcons, player->playerType * 8, 0, 5, 5, xco, yco, 0);
			}
		}
	}

}