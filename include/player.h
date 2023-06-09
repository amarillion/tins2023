#pragma once

#include "object.h"
#include "settings.h"

class Resources;
class Player;
	
class Bullet : public Object
{
	private:
		
		double dx, dy;
		double range; // range in pixels
		
		int damage;
		Player *parent;
	public:
		Player *getPlayer() { return parent; }
		Bullet(Room *r, double angleRad, double speed, int _range, int _damage, Player *_parent);
		
		virtual void update() override;
		virtual void handleCollission(ObjectBase *o);
		static void init(std::shared_ptr<Resources> res);
		
		int getDamage() { return damage; }
		static Anim *bullet[1];
};

class PickUp : public Object
{
public:		
	PickUp(Room *r, int type) : Object (r, type) { 
		solid = true; 
		switch(type) {
			case OT_HEALTH: setAnim(anims[AnimType::HEALTH]); break;
			case OT_HEALTHCONTAINER: setAnim(anims[AnimType::HEALTHCONTAINER]); break;
			case OT_KEY: setAnim(anims[AnimType::KEY]); break;
			case OT_MAP: setAnim(anims[AnimType::MAP]); break;
			case OT_GOLD: setAnim(anims[AnimType::GOLD]); break;
			case OT_BONUS1: setAnim(anims[AnimType::BONUS1]); break;
			case OT_BONUS2: setAnim(anims[AnimType::BONUS2]); break;
			case OT_BONUS3: setAnim(anims[AnimType::BONUS3]); break;
			case OT_BONUS4: setAnim(anims[AnimType::BONUS4]); break;
			default: assert(false);
		}

	}
	virtual void handleCollission(ObjectBase *o);
	static void init(std::shared_ptr<Resources> res);		
	
	enum AnimType { BANANA, HEALTH, KEY, BONUS1, BONUS2, BONUS3, BONUS4, MAP, HEALTHCONTAINER, GOLD, ANIM_NUM };
	static Anim *anims[ANIM_NUM];
};

/**
 * Some helper functions for advanced Objects...
 */
class ObjectMixin : public Object {
public:
	ObjectMixin(Room *r, int type) : Object(r, type) {}
	void drop(int bonusType);
	void say(const std::string &text);
};

class PlayerState
{
	public:
	static const int defaultWpnSpeed = 20;
	static const int defaultWpnRange = 120;
	static const int defaultWpnDamage = 4;
	static const int defaultHpMax = 100;
	
	// player stats	
	int hp = defaultHpMax; // hitpoints
	int hpMax = defaultHpMax; // max hitpoints
	int wpnSpeed = defaultWpnSpeed; // weapon rate of fire
	int wpnRange = defaultWpnRange; // weapon range, distance bullets can go
	int wpnDamage = defaultWpnDamage; 
	int lootTableCounter = 0;
	int wpnType = 0;
	int rescues = 0; // rescues collected
	int keys = 0; // keys collected
	int gold = 0; // gold collected for killing monsters
	bool died = false;
};

class Player : public ObjectMixin
{
	static const int transportDelay = 5;
	static const int invulnerabilityDelay = 50;
	static const int heartHealthValue = 50;
	static constexpr double playerBulletSpeed = 6.0;
	static const int defaultWpnDamage = 4;
	

	static Anim *walk[2];
	static ALLEGRO_SAMPLE *shoot[2][6];
	enum Samples { HURT1, HURT2, UNLOCK, STEPS, PICKUP_KEY, PICKUP_OTHER, TELEPORT, SAMPLE_NUM };
	static ALLEGRO_SAMPLE *samples[SAMPLE_NUM];

	int transportCooldown;
	int hittimer; // invulnerability period...
	int attackCooldown;

	int waitTimer;
	Point waitDelta{0,0};
	std::function<void()> onWaitEnd;
	Input *button;
	
	void hit(int damage);
	
	enum State { CONTROL, ANIMATING };
	State eState = CONTROL;
public:
	int playerType; // 0 for fole, 1 for raul
	PlayerState *ps;
	Player(PlayerState *ps, Room *r, int _playerType);
	virtual void update() override;
	void updateControl();
	static void init(std::shared_ptr<Resources> res);
	virtual void handleCollission(ObjectBase *o);
	void waitMove(int duration, Point delta, int anim, std::function<void(void)> onWaitEnd);
};

