#pragma once

#include <memory>

struct ALLEGRO_BITMAP;
class Level;

class Chart {
public:
	virtual void drawMap(bool mapCollected) = 0;
	virtual ALLEGRO_BITMAP *getBitmap() = 0;
	static std::shared_ptr<Chart> createInstance(Level *level);
};
