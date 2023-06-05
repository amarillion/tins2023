#pragma once

#include "component.h"
#include <array>
#include "chart.h"

class Player;
class Level;
class Game;

class ChartView : public Component {

	Level *level = nullptr;

	ALLEGRO_BITMAP *frame;
	ALLEGRO_BITMAP *mapIcons;

	std::shared_ptr<Chart> chart;
	Game *game;
public:
	explicit ChartView(ALLEGRO_BITMAP *_frame, ALLEGRO_BITMAP *_mapIcons, Game *_game) : frame(_frame), mapIcons(_mapIcons), game(_game) {}

	void draw(const GraphicsContext &gc) override;

	void initLevel(Level *_level) {
		this->level = _level;
		chart = Chart::createInstance(_level);
	}

	void refresh(bool mapCollected) {
		chart->drawMap(mapCollected);
	}
};