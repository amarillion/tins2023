#include <cassert>
#include "mainloop.h"
#include "engine.h"
#include "constants.h"

using namespace std;

int main(int argc, const char *const *argv)
{
	auto mainloop = MainLoop();
	auto engine = Engine::newInstance();

	mainloop
		.setEngine(engine)
		.setAppName("tins23")
		.setTitle("TINS 2023 Entry")
		.setConfigFilename("tins23.cfg")
		.setLogicIntervalMsec(MSEC_PER_TICK)
		.setPreferredGameResolution(GAME_WIDTH, GAME_HEIGHT);

	if (!mainloop.init(argc, argv) && !engine->init())
	{
		mainloop.run();
		engine->done();
	}

	return 0;
}
