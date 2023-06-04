#include "anim.h"
#include "engine.h"
#include "object.h"
#include "player.h"
#include <cassert>
#include "util.h"
#include "game.h"

using namespace std;

Engine *Object::engine;
Game *Object::game;

std::map<int, LegacyDirInfo> LEGACY_DIR = {
	{ UP,    {  0, -1, 'N',  DOWN, M_PI_2 * 3 } },
	{ DOWN,  {  0,  1, 'S',    UP, M_PI_2     } },
	{ LEFT,  { -1,  0, 'W', RIGHT, M_PI       } },
	{ RIGHT, {  1,  0, 'E',  LEFT, 0          } },
};

TEG_MAP *Object::getMap()
{ 
	assert (room);
	return room->map;
}

// tilestack -> check the tiles in all 3 layers at the same time
int Object::getTileStackFlags(int mx, int my)
{
	int result = 0;
	TEG_MAP *map = getMap();
	return tileStackFlags(map, mx, my);
}

Object::Object (Room *r, int aType) : ObjectBase()
{
	assert (r);
	w = 8;
	h = 8;
	dir = RIGHT;
	room = r;
	type = aType;
}

void Object::try_move (double dx, double dy)
{
	int dxleft = dx, dyleft= dy;
	int ddx = dx > 0 ? 1 : -1;
	int ddy = dy > 0 ? 1 : -1;	
	int trydx, trydy;
	bool valid = true;
	while ((abs(dxleft) > 0 || abs (dyleft) > 0) && valid)
	{
		if (abs(dxleft) > abs(dyleft))
		{
			trydy = 0;
			if (abs(dxleft) >= 1)
				trydx = ddx;
			else
				trydx = dxleft;
		}
		else
		{
			trydx = 0;
			if (abs(dyleft) >= 1)
				trydy = ddy;
			else
				trydy = dyleft;
		}

		// check with tilemap background, but only if object is solid.
		if (solid)
		{
			
			// check if (x +  |trydx, y + trydy) is valid
			int mx1, my1, mx2, my2;
			int ix, iy;
			TEG_MAP *map = getMap();
			mx1 = ((int)x + trydx) / map->tilelist->tilew;
			my1 = ((int)y + trydy) / map->tilelist->tileh;
			mx2 = ((int)x + trydx + w - 1) / map->tilelist->tilew;
			my2 = ((int)y + trydy + h - 1) / map->tilelist->tileh;
					
			// loop through all map positions we touch with the solid region
			for (ix = mx1; ix <= mx2; ++ix)
			{
				for (iy = my1; iy <= my2; ++iy)
				{
					// see if there is a solid tile at this position
					if (getTileStackFlags (ix, iy) & (TS_SOLID|TS_OCEAN))
					{
							valid = false;
					}
				}
			}
		}
		
		if (valid)
		{
			x += trydx;
			dxleft -= trydx;
			y += trydy;
			dyleft -= trydy;
		}		
	}
}
   
void Object::setRoom (Room *_room)
{
	assert (_room);	
	room = _room;
}

void Objects::onUpdate()
{
	// collission detection!	
	for (auto i = objects.begin(); i != objects.end(); i++)
		for (auto j = objects.begin(); j != i; j++)
	{
		Object *pi = dynamic_cast<Object *>(*i);
		assert (pi);
		Object *pj = dynamic_cast<Object *>(*j);
		assert (pj);
		// see if bb interesect
		if ((*i)->isAlive() && (*j)->isAlive() && pi->getRoom() == pj->getRoom())
		{
			int x1 = pi->getx();
			int y1 = pi->gety();
			int w1 = pi->getw();
			int h1 = pi->geth();
			int x2 = pj->getx();
			int y2 = pj->gety();
			int w2 = pj->getw();
			int h2 = pj->geth();
			if(!((x1 >= x2+w2) || (x2 >= x1+w1) || (y1 >= y2+h2) || (y2 >= y1+h1)))
			{
				pi->handleCollission (pj);
				pj->handleCollission (pi);
			}
		}		
	}
}

void Objects::draw (const GraphicsContext &gc, Room *room, int cx, int cy, int cw, int ch)
{
	al_set_target_bitmap (gc.buffer);

	int ox, oy, ow, oh;
	al_get_clipping_rectangle(&ox, &oy, &ow, &oh);
	al_set_clipping_rectangle(cx, cy, cw, ch);

	for (auto i = objects.begin(); i != objects.end(); i++)
	{
		Object *pi = dynamic_cast<Object *>(*i);
		assert (pi);

		if (pi->isVisible() && pi->isAlive() &&
			pi->getRoom() == room)
		{
			pi->draw(gc);
		}
	}

	al_set_clipping_rectangle(ox, oy, ow, oh);
}
