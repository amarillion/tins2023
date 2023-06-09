#pragma once

#include <allegro5/allegro.h>
#include <list>
#include <map>
#include "tegel5.h"
#include "level.h"
#include "component.h"
#include "objectbase.h"
#include <cmath>

// tile stack properties
const int TS_SOLID = 0x01;
const int TS_OCEAN = 0x02;

class Objects;
class Engine;
class Anim;
class Resources;

// legacy direction enum
enum { UP, DOWN, LEFT, RIGHT };

enum { 
	OT_MONSTER, OT_BULLET, OT_ENEMY_BULLET, OT_PLAYER, OT_DOOR, OT_LOCKED_DOOR, OT_TELEPORT, 
	
	OT_HEALTH,
	OT_HEALTHCONTAINER,
	OT_KEY,
	OT_BONUS1,
	OT_BONUS2,
	OT_BONUS3,
	OT_BONUS4,
	OT_MAP,
	OT_GOLD,

	OT_DEADMONSTER,
	OT_NO_COLLISION,

	OT_SHOPKEEP,
	OT_RESCUEE
};

struct LegacyDirInfo {
	int dx;
	int dy;
	char shortName;
	int reverse;
	double angle;
};

extern std::map<int, LegacyDirInfo> LEGACY_DIR;

class Room;
class Game;

class Object : public ObjectBase
{
protected:
	static Engine *engine;
	static Game *game;

	Room *room;
	
	int type;
	int getTileStackFlags(int mx, int my);
public:
	Object (Room *r, int type);
	virtual ~Object () {}
	
	TEG_MAP *getMap();
	void try_move (double dx, double dy);
	void setRoom (Room *_room);
	Room *getRoom () { return room; }

	virtual int getType() { return type; }
	static void init(Engine *_engine, Game *_game) { engine = _engine; game = _game; }

};

class Objects : public SimpleObjectList<Object> {
public:
	void onUpdate() override;
	virtual void draw (const GraphicsContext &gc, Room *room, int cx, int cy, int cw, int ch);
	std::list<Object*> getChildren() {
		return objects;
	}
};
