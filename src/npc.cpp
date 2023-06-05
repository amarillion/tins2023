#include "npc.h"
#include "util.h"

using namespace std;

Anim *Shopkeep::sprite = nullptr;
Anim *Rescuee::sprite = nullptr;


void Rescuee::update() {
	if (state == TRAPPED) {
		if (--helpCounter <= 0) {
			say("Help");
			helpCounter = 50 * 8; // 8'000 msec
		}
	}
	else {
	}
}


void Rescuee::handleCollission(ObjectBase *o) {
	switch(o->getType()) {
		case OT_PLAYER: {
			if (state == TRAPPED) {
				rescue();
			}
		}
	}
}

void Rescuee::rescue() {
	state = RESCUED;
	setState(1); // update animation state

	vector<int> choices { OT_BONUS1, OT_BONUS2, OT_BONUS3, OT_BONUS4, OT_HEALTHCONTAINER };
	drop(choice(choices));
	say("Thank you,\nTake this...");
	type = OT_NO_COLLISION; // prevent triggering player again
}

void Shopkeep::update() {}

void Rescuee::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Rescuee");
}

void Shopkeep::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Shopkeep");
}

