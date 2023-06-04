#include "frame.h"

#include <allegro5/allegro5.h>

Point regularFrameFunc(const Point &cell, const Point &frameSize) {
	bool isLeft = cell.x() == 0;
	bool isRight = cell.x() == frameSize.x() - 1;
	bool isTop = cell.y() == 0;
	bool isBottom = cell.y() == frameSize.y() - 1;

	if (isTop) {
		if (isLeft) { return Point {0,0}; }
		else if (isRight) { return Point { 2, 0 }; }
		else return Point { 1, 0 };
	}
	else if (isBottom) {
		if (isLeft) { return Point {0,2}; }
		else if (isRight) { return Point { 2, 2 }; }
		else { return Point { 1, 2 }; }
	}
	else {
		if (isLeft) { return Point {0,1}; }
		else if (isRight) { return Point { 2, 1 }; }
		else return Point { 1, 1 };
	}

}


Point bubbleMapFunc(const Point &cell, const Point &frameSize) {
	bool isLeft = cell.x() == 0;
	bool isRight = cell.x() == frameSize.x() - 1;
	bool isTop = cell.y() == 0;
	bool isBottom = cell.y() == frameSize.y() - 1;

	if (isTop) {
		if (isLeft) { return Point {0,0}; }
		else if (isRight) { return Point { 3, 0 }; }
		else return Point { 1, 0 };
	}
	else if (isBottom) {
		if (isLeft) { return Point {0,3}; }
		else if (isRight) { return Point { 3, 3 }; }
			// special case: arrow
		else {
			int middle = frameSize.x() / 2; // integer division
			if (cell.x() == middle) { return Point { 1, 3 }; }
			else { return Point { 2, 3 }; }
		};
	}
	else {
		if (isLeft) { return Point {0,1}; }
		else if (isRight) { return Point { 3, 1 }; }
		return Point { 1, 1 };
	}
};

Point chartMapFunc(const Point &cell, const Point &frameSize) {
	bool isLeft = cell.x() == 0;
	bool isRight = cell.x() == frameSize.x() - 1;
	bool isTop = cell.y() == 0;
	bool isBottom = cell.y() == frameSize.y() - 1;

	if (isTop) {
		if (isLeft) { return Point {0,0}; }
		else if (isRight) { return Point { 3, 0 }; }
		else return Point { cell.x() % 2 + 1, 0 };
	}
	else if (isBottom) {
		if (isLeft) { return Point {0,3}; }
		else if (isRight) { return Point { 3, 3 }; }
		else return Point  { cell.x() % 2 + 1, 3 };
	}
	else {
		if (isLeft) { return Point {0,cell.y() % 2 + 1}; }
		else if (isRight) { return Point { 3, cell.y() % 2 + 1 }; }
		return Point { 1, 1 };
	}
};

void drawFrame(ALLEGRO_BITMAP *tiles, const Rect &destRect, const Point &tileSize, const std::function<Point(const Point&, const Point&)> &mapFunc) {

	Point tileNum {
		al_get_bitmap_width(tiles) / tileSize.x(),
		al_get_bitmap_height(tiles) / tileSize.y()
	};

	Point frameTileNum {
		destRect.w() / tileSize.x(),
		destRect.h() / tileSize.y()
	};

	for(int yco = 0; yco < frameTileNum.y(); ++yco) {
		for(int xco = 0; xco < frameTileNum.x(); ++xco) {
			Point pos { xco, yco };
			Point tile = mapFunc(pos, frameTileNum);
			al_draw_bitmap_region(
				tiles,
				tile.x() * tileSize.x(), tile.y() * tileSize.y(),
				tileSize.x(), tileSize.y(),
				destRect.x() + xco * tileSize.x(),
				destRect.y() + yco * tileSize.y(),
				0
			);
		}
	}


}