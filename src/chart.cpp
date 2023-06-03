#include "level.h"
#include "color.h"
#include <memory>
#include "chart.h"

using namespace std;

/**
 * Draw a map from a Level.
 */
class ChartImpl: public Chart {
private:
	ALLEGRO_BITMAP *bmp;
	const Level *level;

public:
	ALLEGRO_BITMAP *getBitmap() override { return bmp; }

	ChartImpl(const Level *level): level(level) {
		bmp = al_create_bitmap(level->mw * ROOM_WIDTH, level->mh * ROOM_HEIGHT);

		al_clear_to_color(DARK_GREY);

		al_set_target_bitmap(bmp);
		for (Room *room: level->rooms) {
			// draw a single room...

			int basex = ROOM_WIDTH * room->mx;
			int basey = ROOM_HEIGHT * room->my;

			for (int y = 0; y < ROOM_HEIGHT; ++y) {
				for (int x = 0; x < ROOM_WIDTH; ++x) {
					ALLEGRO_COLOR color;
					int flags = tileStackFlags(room->map, x, y);
					if (flags & TS_SOLID) {
						color = DARK_GREY;
					}
					else {
						color = LIGHT_GREY;
					}
					al_put_pixel(basex + x, basey + y, color);
				}
			}
		}
	}

};

std::shared_ptr<Chart> Chart::createInstance(Level *level) {
	return make_shared<ChartImpl>(level);
}