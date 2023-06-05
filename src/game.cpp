#include "game.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "util.h"
#include "mainloop.h"
#include "strutil.h"
#include "irisshader.h"
#include "object.h"
#include "engine.h"
#include "messages.h"
#include "view.h"
#include "chart.h"
#include "frame.h"
#include "chartview.h"

using namespace std;

class GameImpl : public Game
{
private:
	enum { ICON_CAPY_PLACEHOLDER, ICON_CAPY, ICON_KEY, ICON_NUM };

	RoomSet *roomSet;
	Objects objects;
	Engine *parent;
	Player *player[2];
	PlayerState ps[2];
	std::shared_ptr<View> view[2]; // a view for each player
	ALLEGRO_BITMAP *preProcessing = nullptr;

	int gameTimer;
	Settings *settings;
	Level *level;
	int monsterHp;

	static const int defaultGameTime = 180000; // 180 sec = 3 min
	static const int defaultMonsterHp = 5;
	static const int monsterHpIncrease = 2;
	static const int gameTimeIncrease = 60000;

	bool mapCollected = false;
	int bananaCount;

	unsigned int currentLevel; // index of current Level

	void drawStatus (ALLEGRO_BITMAP *buffer, int x, int y, PlayerState *ps);
	void nextLevel(); // called when player reaches exit
	void initLevel (); // for beginning a new level

	ALLEGRO_FONT *gamefont;
	ALLEGRO_BITMAP* icons[ICON_NUM];

	Input btnPause;
	std::string gameover_message;
	std::shared_ptr<Messages> messages;

	ALLEGRO_BITMAP *chartFrame;
	ALLEGRO_BITMAP *woodFrame;

	shared_ptr<ChartView> chartView;
public:

	void showMessage(const char *str, Messages::Behavior type) override {
		messages->showMessage(str, type);
	}

	const std::string &gameOverMessage() override { return gameover_message; };

	void addTime(int amount) override { gameTimer += amount; messages->showMessage("Extra time", Messages::RIGHT_TO_LEFT); }

	void doneLevel() override; // clean up objects etc.
	void initGame() override;
	int getCurrentLevel() override { return currentLevel; }
	Objects *getObjects() override { return &objects; }
	GameImpl(Engine *engine, Settings *_settings);
	~GameImpl() override;
	void draw(const GraphicsContext &gc) override;
	void update() override;
	Player *getNearestPlayer (Object *o) override;
	void init(std::shared_ptr<Resources> resources) override;

	void collectMap() override {
		mapCollected = true;
		refreshMap();
	}

	void refreshMap() override {
		chartView->refresh(mapCollected);
	}
};

std::shared_ptr<Game> Game::createInstance(Engine *engine, Settings *settings) {
	return make_shared<GameImpl>(engine, settings);
}

GameImpl::GameImpl(Engine *engine, Settings *_settings) : objects(), parent(engine), settings(_settings)
{
	player[0] = nullptr;
	player[1] = nullptr;
	level = nullptr;
	roomSet = nullptr;

	btnPause.setScancode(ALLEGRO_KEY_ESCAPE);

	messages = make_shared<Messages>();
	add(messages);

	view[0] = make_shared<View>(this);	
	view[1] = make_shared<View>(this);
}

GameImpl::~GameImpl()
{
	if (level != NULL) delete level;
	objects.killAll();
	if (roomSet != NULL) delete roomSet;

	for (int i = 0; i < ICON_NUM; ++i) {
		if (icons[i]) al_destroy_bitmap(icons[i]);
	}
}

void GameImpl::draw (const GraphicsContext &gc)
{
	al_set_target_bitmap(gc.buffer);
	al_clear_to_color(LIGHT_BLUE);

	for (int i = 0; i < settings->numPlayers; ++i)
	{
		Rect rect { (int)view[i]->getx() - 8, (int)view[i]->gety() - 8, view[i]->getw() + 16, view[i]->geth() + 16 };
		drawFrame(woodFrame, rect, Point{8,8}, regularFrameFunc);

		if (!ps[i].died)
		{
			view[i]->draw(gc);
		}
		drawStatus (gc.buffer, view[i]->status_x, view[i]->status_y, &ps[i]);
	}

	int min = (gameTimer / 60000);
	int sec = (gameTimer / 1000) % 60;
	int csec = (gameTimer / 10) % 100;
	al_draw_textf (gamefont, WHITE, 0, 464, ALLEGRO_ALIGN_LEFT, "%02i:%02i:%02i", min, sec, csec);

	messages->draw(gc);
	chartView->draw(gc);

//	al_set_target_bitmap(gc2.buffer);
//	al_draw_filled_rectangle(0, 0, MAIN_WIDTH, MAIN_HEIGHT, BLACK);
}

void GameImpl::update()
{
	if (gameTimer % 1000 == 0) {
		if (gameTimer == 60000) {
				messages->showMessage("One minute remaining", Messages::RIGHT_TO_LEFT);
		}

		if (gameTimer == 30000) {
				messages->showMessage("Hurry up!", Messages::RIGHT_TO_LEFT);
		}

		if (gameTimer >= 1000 && gameTimer <= 10000) {
				messages->showMessage(string_format("%i", (gameTimer / 1000) - 1 ), Messages::RIGHT_TO_LEFT);
		}
	}

	messages->update();

	if (btnPause.justPressed())
	{
		pushMsg(Engine::MENU_PAUSE);
	}

	// add objects based on position
	gameTimer -= MainLoop::getMainLoop()->getLogicIntervalMsec();
	objects.update();
	bool gameover = true;
	int bananasGot = 0;

	for (int i = 0; i < settings->numPlayers; ++i)
	{
		view[i]->update();
		
		if (!ps[i].died) gameover = false;
		bananasGot += ps[i].rescues;
	}
	if (gameover)
	{
		gameover_message = "GAME OVER";
		parent->logAchievement(string_format("players_%i_level_%i_gameover", settings->numPlayers, currentLevel));

		pushMsg(Engine::GS_GAME_OVER);
	}
	if (gameTimer <= 0)
	{
		gameover_message = "TIME UP";
		parent->logAchievement(string_format("players_%i_level_%i_timeup", settings->numPlayers, currentLevel));
		pushMsg(Engine::GS_GAME_OVER);
	}
	if (bananaCount - bananasGot == 0)
	{
		doneLevel();
		nextLevel();
		pushMsg(Engine::E_NEXT_LEVEL);
		return;
	}
}

void GameImpl::drawStatus (ALLEGRO_BITMAP *buffer, int x, int y, PlayerState *xps)
{
	// draw rescues
	int bananasGot = 0;
	for (int i = 0; i < settings->numPlayers; ++i) bananasGot += ps[i].rescues;
	int total = bananaCount - bananasGot + xps->rescues;
	for (int i = 0; i < total; ++i) {
		al_draw_bitmap(icons[i < xps->rescues ? ICON_CAPY : ICON_CAPY_PLACEHOLDER], x - 8 + (i * 32), y, 0);
	}
	
	// draw keys
	for (int i = 0; i < xps->keys; ++i) {
		al_draw_bitmap(icons[ICON_KEY], x + 120 + i * 32, y, 0);
	}
	
	// draw health bar
	float x1 = x;
	float y1 = y + 44;
	float hpPerPixel = 0.5;
	float wMax = xps->hpMax * hpPerPixel;
	float wHp = xps->hp * hpPerPixel;
	al_draw_filled_rectangle(x1, y1, x1 + wHp, y1 + 12, RED);
	al_draw_rectangle(x1, y1, x1 + wMax, y1 + 12, WHITE, 1.0);

	al_draw_textf (gamefont, WHITE, x + 128, y + 44, ALLEGRO_ALIGN_LEFT, "XP %05i", xps->xp);
}

void GameImpl::initGame ()
{
	// initialize game and player stats.
	// to start a new game.
	currentLevel = 0;
	gameTimer = defaultGameTime;

	// create a level
	// TODO: dynamic
	ps[0] = PlayerState();
	ps[1] = PlayerState();

	monsterHp = defaultMonsterHp;
	initLevel();
}

Player * initPlayer(PlayerState *ps, Room *room, int i) {
	auto result = new Player (ps, room, i);
	int x, y;
	room->getPlayerLocation(i, &x, &y);
	result->setLocation(x * TILE_SIZE, y * TILE_SIZE);
	cout << "Initializing player " << i << " at [" << x << ", " << y << "]\n";
	room->visited = true;
	return result;
}

void GameImpl::initLevel()
{
	// initialize objects
	level = createLevel(roomSet, &objects, currentLevel + (settings->numPlayers == 1 ? 4 : 6), monsterHp);

	player[0] = initPlayer(&ps[0], level->getStartRoom(0), 0);
	objects.add (player[0]);
	player[0]->say("Let's go!");

	chartView->initLevel(level);
	chartView->addPlayer(0, player[0]);

	if (settings->numPlayers == 2)
	{
		player[1] = initPlayer (&ps[1], level->getStartRoom(1), 1);
		objects.add (player[1]);
		player[1]->say("You bet!");
		chartView->addPlayer(1, player[1]);
	}
	else
	{
		player[1] = nullptr;
	}

	view[0]->init (settings->numPlayers, 0, getw(), geth());
 	view[0]->player = player[0];
	view[1]->init (2, 1, getw(), geth());
	view[1]->player = player[1];

	bananaCount = level->getBananaCount();
	mapCollected = false;

	messages->showMessage(string_format("Rescue %i capybaras", bananaCount), Messages::RIGHT_TO_LEFT);
	chartView->refresh(mapCollected);
}

void GameImpl::doneLevel()
{
	objects.killAll();
	if (level != NULL)
	{
		delete level;
		level = NULL;
	}
}

// called when exit found
void GameImpl::nextLevel()
{
	parent->logAchievement(string_format("players_%i_level_%i", settings->numPlayers, currentLevel));

	// go on to next level
	currentLevel++;
	
	// int max = 0;
	// int maxPlayer = 0;
	// int i;
	// for (i = 0; i < settings->numPlayers; ++i) {
	// 	if (ps[i].rescues > max) {
	// 		maxPlayer = i;
	// 		max = ps[i].rescues;
	// 	}
	// }
	// ps[maxPlayer].hpMax += 10;
	
	for (int i = 0; i < settings->numPlayers; ++i) {
		ps[i].rescues = 0;
		ps[i].keys = 0;
	}

	gameTimer += gameTimeIncrease; // extra minute
	monsterHp += monsterHpIncrease;
	initLevel();
}

Player *GameImpl::getNearestPlayer (Object *o)
{
	assert (o);
	if (settings->numPlayers == 2)
	{
		Room *r1, *r2;
		r1 = player[0]->getRoom();
		r2 = player[1]->getRoom();
		if (r1 == o->getRoom() && r2 == o->getRoom())
		{
			double dx1 = player[0]->getx() - o->getx();
			double dy1 = player[0]->gety() - o->gety();
			double dx2 = player[1]->getx() - o->getx();
			double dy2 = player[1]->gety() - o->gety();
			if (dx1 * dx1 + dy1 * dy1 < dx2 * dx2 + dy2 * dy2)
			{
				return player[1];
			}
			else
			{
				return player[0];
			}
		}
		else if (r1 == o->getRoom())
		{
			return player[0];
		}
		else if (r2 == o->getRoom())
		{
			return player[1];
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		if (player[0]->getRoom() == o->getRoom())
		{
			return player[0];
		}
		else
		{
			return NULL;
		}
	}
}

void GameImpl::init (shared_ptr<Resources> resources) {
	roomSet = RoomSet::init(resources);
	gamefont = resources->getFont("builtin_font")->get();
	messages->setFont(resources->getFont("SpicyRice-Regular")->get(48));

	ALLEGRO_BITMAP *iconsheet = resources->getBitmap("pickups");
	icons[ICON_CAPY_PLACEHOLDER] = al_create_sub_bitmap(iconsheet, 440, 0, 32, 32);
	icons[ICON_CAPY] = al_create_sub_bitmap(iconsheet, 400, 0, 32, 32);
	icons[ICON_KEY] = al_create_sub_bitmap(iconsheet, 80, 0, 40, 40);

	IrisEffect::init(resources);
	preProcessing = al_create_bitmap(MAIN_WIDTH, MAIN_HEIGHT);
	chartFrame = resources->getBitmap("chart");
	woodFrame = resources->getBitmap("frame");

	chartView = make_shared<ChartView>(chartFrame);
	add(chartView);
}