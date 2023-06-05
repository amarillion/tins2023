#include "color.h"
#include "door.h"
#include "engine.h"
#include "monster.h"
#include "npc.h"
#include "object.h"
#include "player.h"
#include "screenshot.h"
#include <cassert>
#include <cstdio>
#include "mainloop.h"
#include <memory>
#include "game.h"
#include "keymenuitem.h"
#include "text.h"
#include "DrawStrategy.h"
#include "strutil.h"
#include "tilemap.h"
#include "versionLoader.h"
#include "metrics.h"
#include "updatechecker.h"
#include "anim.h"
#include "balloon.h"

#ifdef DEBUG
#include "animedit.h"
#endif

using namespace std;

class EngineImpl : public Engine {
private:
	bool debug;

	std::shared_ptr<Resources> resources;
	std::shared_ptr<Metrics> metrics;
//	std::shared_ptr<UpdateChecker> updates;

	Settings settings;
	VersionLoader version;
	Input btnScreenshot;
#ifdef DEBUG
	Input btnAbort;
	Input btnDebugMode;
	Input btnDebugMenu;
#endif
	
	shared_ptr<Game> game;
	
	bool isResume;

	shared_ptr<ToggleMenuItem> miPlayerNum;
	shared_ptr<SliderMenuItem> miSound;
	shared_ptr<SliderMenuItem> miMusic;
	shared_ptr<ActionMenuItem> miStart;

#ifdef DEBUG
	MenuScreenPtr mDebug;
	shared_ptr<AnimEditor> animEdit;
#endif

	MenuScreenPtr mMain;
	MenuScreenPtr mKeys[2];
public:

	EngineImpl() : resources(Resources::newInstance()), settings(), version("data/version.ini")
	{
		debug = false;
		isResume = false;

		btnScreenshot.setScancode (ALLEGRO_KEY_F12);
	#ifdef DEBUG
		btnAbort.setScancode (ALLEGRO_KEY_F10);
		btnDebugMode.setScancode (ALLEGRO_KEY_F11);
		btnDebugMenu.setScancode (ALLEGRO_KEY_F3);
	#endif

		game = Game::createInstance(this, &settings);
		add(game, Container::FLAG_SLEEP);
	}

	~EngineImpl() {
		metrics->done();
	}

	virtual Input* getInput(int p) override { 
		return settings.button[p];
	}

	virtual int init() override
	{
		settings.getFromArgs (MainLoop::getMainLoop()->getOpts());
		settings.getFromConfig(MainLoop::getMainLoop()->getConfig());
		initKeyboard(); // install custom keyboard handler

		const char * directions[] = { "up", "down", "left", "right" };
		Anim::setDirectionModel (make_shared<DirectionModel>(directions, 4));

		try {
			resources->addFiles("data/sfx/*.ogg");
			resources->addFiles("data/*.png");
			resources->addFiles("data/anim.xml");
			resources->addFiles("data/*.ttf");
			resources->addFiles("data/*.tll");
			resources->addFiles("data/*.wav");
			resources->addFiles("data/*.glsl");
			resources->addStream("PearlPollution", "data/music/PearlPollution.ogg");

			vector<string> levels = {
					"map2", "map3", "map4", "map5", "map6"
			};
			//TODO: store tileset reference in map itself.
			for (auto key : levels) {

				auto filename = string_format("data/%s.json", key.c_str());
				resources->addJsonMapFile(key, filename, "tiles2");
				auto amap = resources->getJsonMap(key);
				assert (amap->map->tilelist);
			}
		}
		catch (ResourceException &e) {
			allegro_message ("Could not load all resources with error %s!", e.what());
			return 1;
		}

		ObjectBase::init (&debug, MainLoop::getMainLoop());
		Object::init (this, game.get());
		Player::init(resources);
		Balloon::init(resources);
		Bullet::init(resources);
		Door::init(resources);
		Monster::init(resources);
		PickUp::init(resources);
		Rescuee::init(resources);
		Shopkeep::init(resources);
#ifdef DEBUG
		animEdit = AnimEditor::newInstance();
		add(animEdit, Container::FLAG_SLEEP);
		animEdit->init(resources);
#endif

		game->init(resources);
//		gamefont = resources->getFont("builtin_font")->get();

		srand(time(0));

		std::string versionStr = version.version.get();
		// to prevent malicious manipulation //TODO: should be done in metrics module
		if (versionStr.length() > 50) versionStr = versionStr.substr(0, 50);

		//TODO: can't updateChecker determine cacheDir by itself?
		ALLEGRO_PATH *localAppData = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
		string cacheDir = al_path_cstr(localAppData, ALLEGRO_NATIVE_PATH_SEP);

//		updates = UpdateChecker::newInstance(cacheDir, versionStr, E_QUIT);
//		add (updates, Container::FLAG_SLEEP);
//		updates->start_check_thread("fole", "en");
//		updates->setFont(resources->getFont("builtin_font")->get()); //TODO: should not be necessary

		metrics = Metrics::newInstance("fole", versionStr);
		metrics->logSessionStart();

		initMenu();
		handleMessage(nullptr, COVER);

		return 0;
	}

	void showIntro()
	{
		MainLoop::getMainLoop()->audio()->playMusic(resources->getMusic("PearlPollution"));
		ContainerPtr intro = make_shared<Container>();
		add(intro);
		intro->add(ClearScreen::build(BLACK).get());
		intro->add(Text::buildf(WHITE, "LEVEL %i", game->getCurrentLevel() + 1)
			.center().get());
		if (game->getCurrentLevel() > 0)
		{
			intro->add(Text::buildf(WHITE, "EXTRA TIME", game->getCurrentLevel() + 1)
					.xy(MAIN_WIDTH / 2, MAIN_HEIGHT * 2 / 3).get()); /* TODO .blink(100) */
		}

		intro->setTimer (50, MSG_KILL);
		setFocus(intro);
		setTimer(50, GS_PLAY);
	}

#ifdef DEBUG
	void handleEvent(ALLEGRO_EVENT &event) override {
		if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN) {
			resources->refreshModifiedFiles();
		}
		Engine::handleEvent(event);
	}
#endif

	virtual void handleMessage(ComponentPtr src, int code) override
	{
		switch (code)
		{
		case COVER:
			{
				ContainerPtr intro = make_shared<Container>();
				add(intro);
				intro->add(ClearScreen::build(BLACK).get());
				intro->add(BitmapComp::build(resources->getBitmap("Cover")).layout(Layout::CENTER_MIDDLE_W_H, 0, 0, 256, 192).get());
				intro->setTimer (100, MENU_MAIN);
				intro->setTimer (100, MSG_KILL);
				setFocus(intro);
			}
			break;
		case GS_GAME_OVER:
			{
				game->doneLevel();

				ContainerPtr intro = make_shared<Container>();
				add(intro);
				intro->add(ClearScreen::build(BLACK).get());
				intro->add(Text::buildf(WHITE, "%s", game->gameOverMessage().c_str())
						.center().get());
				initStart();
				intro->setTimer (50, MSG_KILL);
				setFocus(intro);
				setTimer(50, MENU_MAIN);
			}
			break;
		case GS_PLAY:
			setFocus (game);
			break;
		case E_TOGGLE_FULLSCREEN:
			MainLoop::getMainLoop()->toggleWindowed();
			break;
		case E_START_OR_RESUME:
		{
			if (!isResume)
			{
				game->initGame();
				showIntro();
			}
			else
			{
				setFocus(game);
			}
		}
		break;
		case E_NEXT_LEVEL:
		{
			showIntro();
		}
		break;
		case MENU_PAUSE:
			initResume();
			break;
		case MENU_PLAYER_NUM:
			if (!isResume)
			{
				settings.numPlayers = 3 - settings.numPlayers;
				set_config_int (MainLoop::getMainLoop()->getConfig(), "fole1", "numplayers", settings.numPlayers);
			}
			break;
		case MENU_KEYS_1:
			setFocus (mKeys[0]);
			break;
		case MENU_KEYS_2:
			setFocus (mKeys[1]);
			break;
#ifdef DEBUG
		case MENU_DEBUG:
			setFocus (mDebug);
			break;
		case ANIM_EDIT:
			setFocus (animEdit);
			break;
#endif
		case E_EXITSCREEN:
//			setFocus(updates);
			metrics->logSessionClose();
			pushMsg(MSG_CLOSE);
			break;
		case E_QUIT:
			pushMsg(MSG_CLOSE);
			break;
		case MENU_MAIN:
			setFocus (mMain);
			break;
		}
	}

	virtual void onUpdate () override
	{
		//TODO: implement these buttons as children of mainloop that send action events on press
		if (btnScreenshot.justPressed())
		{
			screenshot();
		}

#ifdef DEBUG
		if (btnDebugMode.justPressed()) {
			debug = !debug;
		}
		if (btnDebugMenu.justPressed()) {
			setFocus(mDebug);
		}
		if (btnAbort.justPressed()) {
			pushMsg(E_QUIT);
		}
#endif
	}


	void initMenu()
	{
		//TODO
		// setMargins (160, 100);

		//TODO
		//sound_cursor = resources->getSample ("pong");
		//sound_enter = resources->getSample ("mugly2");

	//	ALLEGRO_BITMAP *cover = resources->getBitmap("cover");

		//TODO: used to be font "Metro"
		//menufont = resources->getAlfont("Vera", 24);
		sfont = resources->getFont("SpicyRice-Regular")->get(24);

		auto audio = MainLoop::getMainLoop()->audio();
		miSound = make_shared<SliderMenuItem>(&audio->soundVolume,
											  "Sfx", "Press left or right to change sound volume");
		miSound->setEnabled(audio->isInstalled());

		miMusic = make_shared<SliderMenuItem>(&audio->musicVolume,
											  "Music", "Press left or right to change music volume");
		miMusic->setEnabled(audio->isInstalled());

		miPlayerNum = make_shared<ToggleMenuItem>(MENU_PLAYER_NUM,
				"2 Player mode", "1 Player mode", "press enter to change number of players");
		miPlayerNum->setToggle(settings.numPlayers == 2);

		miStart = make_shared<ActionMenuItem>(E_START_OR_RESUME, "Start game", "");
		mMain = MenuBuilder(this, NULL)
			.push_back (miStart)
			.push_back (miSound)
			.push_back (miMusic)
			.push_back (miPlayerNum)
			.push_back (make_shared<ActionMenuItem>(MENU_KEYS_1, "Configure player 1 keys", ""))
			.push_back (make_shared<ActionMenuItem>(MENU_KEYS_2, "Configure player 2 keys", ""))
			.push_back (make_shared<ActionMenuItem>(E_TOGGLE_FULLSCREEN, "Toggle Fullscreen", "Switch fullscreen / windowed mode"))
			.push_back (make_shared<ActionMenuItem>(E_EXITSCREEN, "Quit", ""))
			.build();
		mMain->add(ClearScreen::build(BLACK).get(), FLAG_BOTTOM);
		mMain->setMargin(160, 80);
	//	mMain->add(BitmapComp::build(cover).xy((MAIN_WIDTH - al_get_bitmap_width(cover)) / 2, 80).get());

		mMain->add(Text::build(GREY, ALLEGRO_ALIGN_RIGHT, string_format("v%s.%s", version.version.get(), version.buildDate.get()))
				.layout(Layout::RIGHT_BOTTOM_W_H, 4, 4, 200, 28).get());

		for (int i = 0; i < 2; ++i)
		{
			ALLEGRO_CONFIG *config = MainLoop::getMainLoop()->getConfig();
			mKeys[i] = MenuBuilder (this, NULL)
				.push_back (make_shared<KeyMenuItem> ("left", config_keys[i][0], settings.getInput(i)[btnLeft], config))
				.push_back (make_shared<KeyMenuItem> ("right", config_keys[i][1], settings.getInput(i)[btnRight], config))
				.push_back (make_shared<KeyMenuItem> ("down", config_keys[i][2], settings.getInput(i)[btnDown], config))
				.push_back (make_shared<KeyMenuItem> ("up", config_keys[i][3], settings.getInput(i)[btnUp], config))
				.push_back (make_shared<KeyMenuItem> ("action", config_keys[i][4], settings.getInput(i)[btnAction], config))
				.push_back (make_shared<KeyMenuItem> ("alt", config_keys[i][5], settings.getInput(i)[btnAlt], config))
				.push_back (make_shared<ActionMenuItem> (MENU_MAIN, "Return", "Return to main menu"))
				.build();
			mKeys[i]->add(ClearScreen::build(BLACK).get(), FLAG_BOTTOM);
			mKeys[i]->setMargin(160, 80);
	//		mKeys[i]->add(BitmapComp::build(cover).xy((MAIN_WIDTH - al_get_bitmap_width(cover)) / 2, 80).get());
		}

#ifdef DEBUG
		mDebug = MenuBuilder(this, nullptr)
				.push_back (make_shared<ActionMenuItem>(ANIM_EDIT, "Animation viewer", ""))
				.push_back (make_shared<ActionMenuItem> (MENU_MAIN, "Return", "Return to main menu"))
				.build();
		mDebug->add(ClearScreen::build(BLACK).get(), FLAG_BOTTOM);
		mDebug->setMargin(160, 80);
#endif
	}

	void initStart() {
		setFocus(mMain);
		miStart->setText("Start");
		miPlayerNum->setEnabled(true);
		isResume = false;
	}

	void initResume() {
		setFocus(mMain);
		miStart->setText("Resume");
		miPlayerNum->setEnabled(false);
		isResume = true;
	}

	virtual void logAchievement(const std::string &achievement) override {
		metrics->logAchievement(achievement);
	}

	virtual bool isDebug () override { return debug; }
};

shared_ptr<Engine> Engine::newInstance() {
	return make_shared<EngineImpl>();
}