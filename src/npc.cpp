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
	setState(1); // animation state -> fishnet removed
	say("Thank you!");
	type = OT_NO_COLLISION; // prevent triggering player again
}

Shopkeep::Shopkeep(Room *r) : ObjectMixin(r, OT_SHOPKEEP) {
	setVisible(true);
	solid = true;
	setAnim(sprite);
	vector<int> choices { OT_BONUS1, OT_BONUS2, OT_BONUS3, OT_BONUS4, OT_HEALTHCONTAINER };
	bonus = choice(choices);
	switch(bonus) {
		case OT_BONUS1:
			price = 60;
			message = "Extra time\nfor $60";
			break;
		case OT_BONUS2:
			price = 50;
			message = "Extra soaker pressure\nfor $50";
			break;
		case OT_BONUS3:
			price = 120;
			message = "Soaker upgrade\nfor $120";
			break;
		case OT_BONUS4:
			price = 100;
			message = "Extra time for $100";
			break;
		case OT_HEALTHCONTAINER:
			price = 100;
			message = "Extra max health\nfor $100";
			break;
	}
}

void Shopkeep::handleCollission(ObjectBase *o) {
	if (sold) return;

	switch(o->getType()) {
		case OT_PLAYER: {
			if (cooldown == 0) {
				Player *player = dynamic_cast<Player*>(o);
				if (player) {
					if (player->ps->gold < price) {
						say("Not enough gold!");
					}
					else {
						say("Sold!");
						player->ps->gold -= price;
						drop(bonus);
						sold = true;
					}
				}
			}
			cooldown = 50;
		}
	}
}

void Shopkeep::update() {
	if (sold) return;

	if (cooldown > 0) cooldown--;
	--helpCounter;

	if (helpCounter <= 0 && cooldown == 0) {
		say(message);
		helpCounter = 50 * 8; // 8'000 msec
	}
}

void Rescuee::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Rescuee");
}

void Shopkeep::init(std::shared_ptr<Resources> res) {
	sprite = res->getAnim("Shopkeep");
}
