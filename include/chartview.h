#pragma once

#import "component.h"
#include <array>
#include "chart.h"

class Player;
class Level;

class ChartView : public Component {

	std::array<Player*, 2> player {nullptr, nullptr };
	Level *level = nullptr;

	ALLEGRO_BITMAP *frame;
	std::shared_ptr<Chart> chart;
public:
	explicit ChartView(ALLEGRO_BITMAP *_frame) : frame(_frame) {}

	void draw(const GraphicsContext &gc) override;

	void initLevel(Level *_level) {
		this->level = _level;
		chart = Chart::createInstance(_level);
		player[0] = nullptr;
		player[1] = nullptr;
	}

	void addPlayer(int idx, Player *p) {
		player[idx] = p;
	}

	void refresh(bool mapCollected) {
		chart->drawMap(mapCollected);
	}
};