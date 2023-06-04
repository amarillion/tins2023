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

	void rescue() {
		state = RESCUED;
		setState(1); // update animation state
		drop(OT_BONUS1);
		say("Thank you!");
		type = OT_NO_COLLISION; // prevent triggering player again
	}

	void update() override;
};

class Shopkeep : public Object {
	static Anim *sprite;
public:
	Shopkeep(Room *r) : Object(r, OT_SHOPKEEP) {
		setVisible(true);
		solid = true;
		setAnim(sprite);
	}
	static void init(std::shared_ptr<Resources> res);

	void handleCollission(ObjectBase *o) override {}
	void update() override;
};
