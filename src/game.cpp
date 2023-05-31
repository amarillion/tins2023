#include "game.h"
#include <cassert>
#include "animator.h"
#include "engine.h"
#include "text.h"
#include "resources.h"
#include "DrawStrategy.h"
#include "viewport.h"
#include "util.h"
#include "textstyle.h"
#include "mainloop.h"
#include "input.h"

using namespace std;

class GameImpl : public Game
{
	Engine *parent;

public:
	GameImpl(Engine *parent) : 
			parent(parent)
	{}

	virtual ~GameImpl() {
	}

	virtual void init() override {
	}

	virtual void done() override {

	}

	Input bEsc { ALLEGRO_KEY_ESCAPE };
	Input bCheat { ALLEGRO_KEY_F9 };

	std::map<SpriteEx *, std::map <SpriteEx *, int > > collisions;

public:
	void initGame() override;
	void onUpdate() override;
	void draw(const GraphicsContext &gc) override;

	virtual Engine *getParent() { return parent; }

private:
};

void GameImpl::onUpdate () {
	if (bEsc.justPressed()) {
		pushMsg(Engine::E_SHOW_GAME_OVER);
	}
}

void GameImpl::draw (const GraphicsContext &gc) {
	al_clear_to_color(BLUE);
}

void GameImpl::initGame()
{
}

shared_ptr<Game> Game::newInstance(Engine *parent)
{
	return make_shared<GameImpl>(parent);
}
