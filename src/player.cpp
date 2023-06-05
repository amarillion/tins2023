#include <cassert>
#include "player.h"
#include "anim.h"
#include "engine.h"
#include "door.h"
#include "game.h"
#include "mainloop.h"
#include "monster.h"
#include <cmath>
#include "balloon.h"

using namespace std;

Anim *Bullet::bullet[1];

Bullet::Bullet(Room *r, double angleRad, double _speed, int _range, int _damage, Player *_parent) 
	: Object (r, _parent ? OT_BULLET : OT_ENEMY_BULLET) 
{
	solid = false;
	setAnim (bullet[0], type == OT_BULLET ? 0 : 1);
	dx = cos(angleRad) * _speed;
	dy = sin(angleRad) * _speed;
	range = _range / _speed; // normalized
	damage = _damage;
	parent = _parent;
}

void Bullet::init(std::shared_ptr<Resources> res)
{
	bullet[0] = res->getAnim("bullet");
}

void Bullet::update()
{
	try_move (dx, dy);
	range -= 1.0; // We assume that speed is constant over the lifetime of the bullet

	if (range <= 0) 
	{
		kill();
		return;
	}

	// no map collision
}


void Bullet::handleCollission(ObjectBase *o)
{
	if (type == OT_BULLET && o->getType() == OT_MONSTER) // bullet hits monster
	{
		kill();
	}
	if (type == OT_ENEMY_BULLET && o->getType() == OT_PLAYER) // enemy bullet hits player
	{
		kill();
	}
}
	
void PickUp::handleCollission(ObjectBase *o)
{
	if (o->getType() == OT_PLAYER)
	{
		kill();
	}
}

void PickUp::init(std::shared_ptr<Resources> res)
{
	anims[AnimType::BANANA] = res->getAnim("banana");
	anims[AnimType::HEALTH] = res->getAnim("health");
	anims[AnimType::GOLD] = res->getAnim("gold");
	anims[AnimType::KEY] = res->getAnim("key");
	anims[AnimType::BONUS1] = res->getAnim("bonus1");
	anims[AnimType::BONUS2] = res->getAnim("bonus2");
	anims[AnimType::BONUS3] = res->getAnim("bonus3");
	anims[AnimType::BONUS4] = res->getAnim("bonus4");
	anims[AnimType::HEALTHCONTAINER] = res->getAnim("healthcontainer");
	anims[AnimType::MAP] = res->getAnim("map");
}

Anim *PickUp::anims[PickUp::AnimType::ANIM_NUM];

void ObjectMixin::drop(int bonusType) {
	// instantiate a bonus...
	PickUp *pickup = new PickUp(getRoom(), bonusType);
	game->getObjects()->add (pickup);
	pickup->setLocation (getx() + 8, gety());
}

void ObjectMixin::say(const string &text) {
	Balloon *balloon = new Balloon(this->room, this, text);
	balloon->updatePositionToParent();
	game->getObjects()->add(balloon);
}

Player::Player(PlayerState *_ps, Room *r, int _playerType) : ObjectMixin(r, OT_PLAYER)
{
	ps = _ps;
	transportCooldown = 0;
	setVisible(true);
	hittimer = invulnerabilityDelay; // start invulnerable, because you could start near an enemy
	attackCooldown = 0;
	button = NULL;
	playerType = _playerType;
	solid = true;
	
	setAnim (walk[playerType]);	
}

Anim *Player::walk[2];
ALLEGRO_SAMPLE *Player::samples[Player::SAMPLE_NUM];
ALLEGRO_SAMPLE *Player::shoot[2][6];

void Player::init(std::shared_ptr<Resources> res)
{
	walk[0] = res->getAnim ("fole_walk");
	walk[1] = res->getAnim ("raul_walk");

 	samples[HURT1] = res->getSample ("miau2");
 	samples[HURT2] = res->getSample ("uh");
 	samples[PICKUP_KEY] = res->getSample ("Pick_up_key_2");
 	samples[PICKUP_OTHER] = res->getSample ("Pick_up_key_1");
 	samples[UNLOCK] = res->getSample ("Door_unlock");
 	samples[STEPS] = res->getSample ("Footsteps_double");
 	samples[TELEPORT] = res->getSample ("AlienSiren");

	shoot[1][0] = res->getSample ("Strum_1");
	shoot[1][1] = res->getSample ("Strum_2");
	shoot[1][2] = res->getSample ("Strum_3");
	shoot[1][3] = res->getSample ("Strum_4");
	shoot[1][4] = res->getSample ("Strum_5");
	shoot[1][5] = res->getSample ("Strum_6");
	shoot[0][0] = res->getSample ("Hit1");
	shoot[0][1] = res->getSample ("Hit2");
	shoot[0][2] = res->getSample ("Hit3");
	shoot[0][3] = res->getSample ("Hit4");
	shoot[0][4] = res->getSample ("Hit5");
	shoot[0][5] = res->getSample ("Hit6");

}

void Player::waitMove(int duration, Point delta, int anim, std::function<void()> _onWaitEnd) {
	type = OT_NO_COLLISION;
	eState = ANIMATING;
	waitTimer = duration;
	setState(anim);
	onWaitEnd = std::move(_onWaitEnd);
	waitDelta = delta;
}

void Player::update()
{
	if (eState == CONTROL) {
		updateControl();
	}
	else {
		waitTimer--;
		x += waitDelta.x();
		y += waitDelta.y();
		if (waitTimer < 0) {
			setState(0); // back to regular animation...
			type = OT_PLAYER;
			eState = CONTROL;
			onWaitEnd();
		}
	}
}

void Player::updateControl()
{
	if (!button) button = engine->getInput(playerType);
	assert (button);

	// reduce transportCooldown
	// when touching a transport (door or teleport), transportCooldown is increased again in handlecollision.
	if (transportCooldown > 0)
	{
		transportCooldown--;
	}
	
	if (hittimer > 0)
	{
		hittimer--;
		setVisible (!(hittimer & 0x02));
	}
	
	bool freshAnim = false;
		
	// attacking
	if (attackCooldown > 0)
	{
		attackCooldown --;
	}
	else
	{
		if (button[btnAction].getState())
		{
			attackCooldown = ps->wpnSpeed;
			int idx = 5;
			MainLoop::getMainLoop()->audio()->playSample(shoot[ps->wpnType == 0 ? 0 : 1][idx]);
	
			double angle = LEGACY_DIR[getDir()].angle;

			// generate bullet...
			double nozx = getx() + LEGACY_DIR[getDir()].dx * 12;
			double nozy = gety() + LEGACY_DIR[getDir()].dy * 12;

			double angleDelta[] = { 0, -0.2, 0.2, -M_PI_2, M_PI_2 };
			int bulletNum;
			switch (ps->wpnType) {
				case 0: bulletNum = 1; break;
				case 1: bulletNum = 3; break;
				default: bulletNum = 5; break;
			}

			for (int i = 0; i < bulletNum; ++i) {
				double delta = angleDelta[i];
				Bullet *bullet = new Bullet(getRoom(), angle + delta, playerBulletSpeed, ps->wpnRange, ps->wpnDamage, this);
				bullet->setDir(getDir());
				game->getObjects()->add (bullet);
				bullet->setLocation (nozx, nozy);
			}
		}
	}
	
	double dx = 0, dy = 0;

	const double speedFactor = 3;
	
	int newDir = -1;
	
	if (button[btnLeft].getState()) {
		dx = -speedFactor;
		newDir = LEFT;
	} 
	else if (button[btnRight].getState()) {
		dx = speedFactor;
		newDir = RIGHT;
	}

	try_move (dx, 0);
	
	if (button[btnUp].getState()) {
		dy = -speedFactor;
		newDir = UP;
	}
	else if (button[btnDown].getState()) {
		dy = speedFactor;
		newDir = DOWN;
	} 

	try_move (0, dy);

	// walking or not?
	if (newDir != -1) {
		if (getDir() != newDir) {
			setDir(newDir);
			setAnim (walk[playerType]);
		}
	}

	// handle animation
	Object::update();
}

void Player::hit(int damage)
{
	hittimer = invulnerabilityDelay;
	
	MainLoop::getMainLoop()->audio()->playSample(samples[playerType == 0 ? HURT1 : HURT2]);
	
	ps->hp -= damage;
	if (ps->hp <= 0)
	{
		// drop keys, they are essential objects for the other player
		while (ps->keys > 0) {
			ps->keys--;
			auto key = new PickUp(getRoom(), OT_KEY);
			game->getObjects()->add (key);
			key->setLocation (getx() + random(20) - 10, gety() + random(20) - 10);
		}

		ps->died = true;
		setVisible (false);
		solid = false;
		for (int i = 0; i < ps->keys; ++i) {
			drop(OT_KEY);
		}

		kill(); // TODO: dying animation
	}	
}

void Player::handleCollission (ObjectBase *o)
{
	if (ps->died) return;
	
	switch(o->getType()) {
		case OT_RESCUEE: {
			MainLoop::getMainLoop()->audio()->playSample (samples[PICKUP_OTHER]);
			ps->rescues++;
		}
		break;
		case OT_KEY: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_KEY]);
			ps->keys++;
		}
		break; 
		case OT_HEALTH: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			ps->hp = std::min(ps->hp + heartHealthValue, ps->hpMax);
		}
		break;
		case OT_MAP: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			game->collectMap();
		}
		break;
		case OT_HEALTHCONTAINER: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			ps->hpMax += 25;
			ps->hp = ps->hpMax;
			say("Max Health Up");
		}
		break;
		case OT_GOLD: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_KEY]);
			ps->gold += 10;
		}
		break;
		case OT_BONUS1: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			game->addTime(30000); // add 30 seconds
		}
		break;
		case OT_BONUS2: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			ps->wpnRange += 40;
			say("Soaker pressure up");
		}
		break; 
		case OT_BONUS3: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			ps->wpnDamage += 2;
			say("Soaker damage up");
		}
		break; 
		case OT_BONUS4: {
			MainLoop::getMainLoop()->audio()->playSample(samples[PICKUP_OTHER]);
			ps->wpnType++;
			say("Got soaker upgrade");
		}
		break; 
		case OT_MONSTER: { // monster
			Monster *m = dynamic_cast<Monster*>(o);
			if (hittimer == 0) hit(m->getDamage());
		}
		break;
		case OT_LOCKED_DOOR: {
			if (ps->keys > 0) {
				Door *d = dynamic_cast<Door*>(o);			
				d->setLocked(false);
				MainLoop::getMainLoop()->audio()->playSample(samples[UNLOCK]);
				ps->keys--;
				waitMove(50, Point{0,0}, 0, [=](){});
				// TODO puff of smoke animation
			}
			else {
				if (transportCooldown == 0)
				{
					say("I need a key!");
				}
				transportCooldown = transportDelay; // make sure we don't go back
			}
		}	
		break; 
		case OT_DOOR: { // exit
			if (transportCooldown == 0)
			{
				Door *d = dynamic_cast<Door*>(o);
				assert (d);
				MainLoop::getMainLoop()->audio()->playSample(samples[STEPS]);

				Point dir {
						LEGACY_DIR[d->getDir()].dx * 4,
						LEGACY_DIR[d->getDir()].dy * 4
				};
				setDir(d->getDir());
				waitMove(25, dir, 0, [=, this](){
					if (d->otherRoom != nullptr) {
						setRoom(d->otherRoom);
						d->otherRoom->visited = true;
						game->refreshMap();
						setLocation(d->otherDoor->getx(), d->otherDoor->gety());
					}
					hittimer = invulnerabilityDelay;
				});
			}
			transportCooldown = transportDelay; // make sure we don't go back
		}
		break; 
		case OT_TELEPORT: {
			if (transportCooldown == 0)
			{
				Door *t = dynamic_cast<Door*>(o);
				assert (t);
				MainLoop::getMainLoop()->audio()->playSample(samples[TELEPORT]);
				waitMove(50, Point{0,0}, 1, [=, this]() {
					if (t->otherRoom != nullptr) {
						setRoom(t->otherRoom);
						t->otherRoom->visited = true;
						game->refreshMap();
						setLocation(t->otherDoor->getx(), t->otherDoor->gety());
					}
					hittimer = invulnerabilityDelay;
				});
			}
			transportCooldown = transportDelay; // make sure we don't go back
		}
		break; 
		case OT_ENEMY_BULLET: {		
			Bullet *b = dynamic_cast<Bullet*>(o);
			if (hittimer == 0) hit(b->getDamage());
		}
	}
}
