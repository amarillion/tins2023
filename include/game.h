#pragma once

#include "state.h"

class Engine;
struct TEG_MAP;
struct ALLEGRO_FONT;
class SpriteEx;

class Game : public State
{
public:
	virtual void init() = 0;
	virtual void done() = 0;
	virtual void initGame() = 0;

	static std::shared_ptr<Game> newInstance(Engine *parent);
};
