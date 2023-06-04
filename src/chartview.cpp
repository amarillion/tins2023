#include "chartview.h"
#include "color.h"
#include "rect.h"
#include "frame.h"
#include "settings.h"
#include "level.h"
#include "player.h"

void ChartView::draw(const GraphicsContext &gc) {
	const int basex = 8;
	const int basey = 8;

	Rect chartRect { 0, 0, 128, 96 };
	drawFrame(frame, chartRect, Point{8,8}, chartMapFunc);
	al_draw_bitmap(chart->getBitmap(), basex, basey, 0);

	for (int i = 0; i < 2; ++i) {
		if (player[i] != nullptr) {
			int xco = basex + player[i]->getRoom()->mx * ROOM_WIDTH + (player[i]->getx() / TILE_SIZE);
			int yco = basey + player[i]->getRoom()->my * ROOM_HEIGHT + (player[i]->gety() / TILE_SIZE);
			al_put_pixel(xco, yco, i == 0 ? BLUE : GREEN);
		}
	}
}