#pragma once

#include "object.h"
#include "player.h"

class Rescuee : public ObjectMixin {
	static Anim *sprite;

	int helpCounter = 0;

	enum State { TRAPPED, RESCUED };
	int state = TRAPPED;
public:
	Rescuee(Room *r) : ObjectMixin(r, OT_RESCUEE) {
		setVisible(true);
		solid = true;
		setAnim(sprite);
	}

	static void init(std::shared_ptr<Resources> res);
	void handleCollission(ObjectBase *o) override;

	void rescue();
	void update() override;
};

class Shopkeep : public ObjectMixin {
	static Anim *sprite;
	int helpCounter = 0;
	int cooldown = 0;
	int bonus = 0;
	std::string message {};
	int price = 0;
	bool sold = false;
public:
	Shopkeep(Room *r);
	static void init(std::shared_ptr<Resources> res);

	void handleCollission(ObjectBase *o) override;
	void update() override;
};
