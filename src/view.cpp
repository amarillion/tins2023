#include <cassert>
#include "view.h"
#include "game.h"
#include "mainloop.h"

using namespace std;

void View::init(int numPlayers, int player, int pw, int ph)
{
	int HALF_W = pw / 2;

	const int defaults[3][6] =
	{
		{ 8, 96+8,          pw - 16, ph - (96+16),                128, 0 },
		{ HALF_W + 8, 8, HALF_W - 16, 368,                   HALF_W + 8, 368+16 },
		{ 8, 96+8,            HALF_W - 16, 368,                   128, 0 },
	};
	camera_x = 0;
	camera_y = 0;
	
	int i = (numPlayers == 1 ? 0 : player + 1);
	
	x = defaults[i][0];
	y = defaults[i][1];
	w = defaults[i][2];
	h = defaults[i][3];
	status_x = defaults[i][4];
	status_y = defaults[i][5];

	iris = make_unique<IrisEffect>();
}

void View::update() {
	// update camera
	int newx, newy;
	newx = player->getx() - (w / 2);
	newy = player->gety() - (h / 2);
	camera_x = newx;
	camera_y = newy;
}

void View::draw(const GraphicsContext &gc) {
	TEG_MAP *map = player->getMap();
	teg_partdraw (map, 0,
			x, y, w, h,
			camera_x - x, camera_y - y);

	int counter = MainLoop::getMainLoop()->getMsecCounter();
	iris->enable((float)counter / 1000.0f, camera_x, camera_y);
	teg_partdraw (map, 1,
			x, y, w, h,
			camera_x - x, camera_y - y);
	iris->disable();

	teg_partdraw (map, 2,
				  x, y, w, h,
				  camera_x - x, camera_y - y);
	GraphicsContext gc2;
	gc2.buffer = gc.buffer;
	gc2.xofst = camera_x - x;
	gc2.yofst = camera_y - y;
	parent->getObjects()->draw(gc2, player->getRoom(), x, y, w, h);
}