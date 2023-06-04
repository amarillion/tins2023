#pragma once

#include <functional>
#include "point.h"
#include "rect.h"

struct ALLEGRO_BITMAP;

Point chartMapFunc(const Point &cell, const Point &frameSize);
Point bubbleMapFunc(const Point &cell, const Point &frameSize);

/**
 * Slices input tiles in equal tiles of size tileSize.
 * @param tiles bitmap holding tiles
 * @param destRect rectangle in pixels where the frame will be drawn.
 * @param tileSize size of each tile.
 * @param mapFunc determines with tile (x, y) to be drawn for each cell(x,y) related to size(x,y)
 */
void drawFrame(ALLEGRO_BITMAP *tiles, const Rect &destRect, const Point &tileSize, const std::function<Point(const Point &, const Point &)> &mapFunc);