#pragma once

#include "tegel5.h"
#include "player.h"
#include "component.h"
#include "irisshader.h"

class Game;

class View : public Component
{
	private:
		int camera_x, camera_y;
		std::unique_ptr<IrisEffect> iris;
	public:
		View(Game *parent) : parent(parent) {}
		void init (int numPlayers, int player, int pw, int ph);
		
		int status_x, status_y;
		Player *player;
		Game *parent;

		virtual void draw(const GraphicsContext &gc) override;
		virtual void update() override;
};
