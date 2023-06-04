#pragma once
#include "setting.h"

const int GAME_WIDTH=800;
const int GAME_HEIGHT=480;

struct ALLEGRO_CONFIG;

class Constants
{
public:
	// allow overriding start weapon type for debugging
	Setting<int, Constants> playerStartWeaponType { this, "fole", "playerStartWeaponType", 0 };

	Constants() {}
	ALLEGRO_CONFIG *getConfig();

	// game options
//	bool fpsOn;
//	bool windowed;
	

private:	
//	bool soundOn;
//	bool music;
};

