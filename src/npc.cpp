#include "npc.h"

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


void Shopkeep::update() {}

void Rescuee::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Rescuee");
}

void Shopkeep::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Shopkeep");
}

