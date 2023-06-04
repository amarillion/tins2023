#include "level.h"
#include "color.h"
#include <memory>
#include "chart.h"
#include <allegro5/allegro_primitives.h>

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

	explicit ChartImpl(const Level *level): level(level) {
		bmp = al_create_bitmap(level->mw * ROOM_WIDTH, level->mh * ROOM_HEIGHT);

		al_set_target_bitmap(bmp);
		al_clear_to_color(TRANSPARENT);

		for (Room *room: level->rooms) {
			// draw a single room...

			int basex = ROOM_WIDTH * room->mx;
			int basey = ROOM_HEIGHT * room->my;

			for (int y = 0; y < ROOM_HEIGHT; ++y) {
				for (int x = 0; x < ROOM_WIDTH; ++x) {
					ALLEGRO_COLOR color;
					int flags = tileStackFlags(room->map, x, y);
					if (flags & TS_SOLID) {
						al_put_pixel(basex + x, basey + y, DARK_GREY);
					}
					else if (flags & TS_OCEAN) {
						// leave transparent...
					}
					else {
						al_put_pixel(basex + x, basey + y, LIGHT_GREY);
					}
				}
			}

			// draw doors...
			int flags = room->getFlags();
			int ROOM_WIDTH_2 = ROOM_WIDTH / 2;
			int ROOM_HEIGHT_2 = ROOM_HEIGHT / 2;
			if (flags & INIT_DOOR_N) {
				al_draw_filled_rectangle(basex + ROOM_WIDTH_2 - 1, basey + 0, basex + ROOM_WIDTH_2 + 1, basey + 2,
										 flags & INIT_LOCK_N ? WHITE: LIGHT_GREY);
			}
			if (flags & INIT_DOOR_E) {
				al_draw_filled_rectangle(basex + ROOM_WIDTH - 2, basey + ROOM_HEIGHT_2 - 1, basex + ROOM_WIDTH, basey + ROOM_HEIGHT_2 + 1,
										 flags & INIT_LOCK_E ? WHITE: LIGHT_GREY);
			}
			if (flags & INIT_DOOR_S) {
				al_draw_filled_rectangle(basex + ROOM_WIDTH_2 - 1, basey + ROOM_HEIGHT - 2, basex + ROOM_WIDTH_2 + 1, basey + ROOM_HEIGHT,
										 flags & INIT_LOCK_S ? WHITE: LIGHT_GREY);
			}
			if (flags & INIT_DOOR_W) {
				al_draw_filled_rectangle(basex + 0, basey + ROOM_HEIGHT_2 - 1, basex + 2, basey + ROOM_HEIGHT_2 + 1,
										 flags & INIT_LOCK_W ? WHITE: LIGHT_GREY);
			}
		}
	}

	virtual ~ChartImpl() = default;
};

std::shared_ptr<Chart> Chart::createInstance(Level *level) {
	return make_shared<ChartImpl>(level);
}