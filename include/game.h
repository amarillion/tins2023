#pragma once

#include "state.h"
#include <memory>
#include "messages.h"

class IrisEffect;
class Engine;
class Settings;
class Resources;
class Object;
class Objects;
class Player;

class Game : public State {
public:
	virtual bool isMapCollected() = 0;
	virtual void collectMap() = 0;
	virtual void refreshMap() = 0;
	virtual void initGame() = 0;
	virtual void doneLevel() = 0;
	virtual void init(std::shared_ptr<Resources> resources) = 0;
	virtual const std::string &gameOverMessage() = 0;
	virtual int getCurrentLevel() = 0;
	static std::shared_ptr<Game> createInstance(Engine *engine, Settings *settings);
	virtual Player *getNearestPlayer(Object *o) = 0;
	virtual Objects *getObjects() = 0;
	virtual void addTime(int msec) = 0;
	virtual void showMessage(const char *value, Messages::Behavior) = 0;
};
