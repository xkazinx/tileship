#pragma once

class Map;

class Game
{
public:
	Game(game::ID id);

	void EnterGame(Player* player);

	void ExitGame(Player* player);

	game::ID _id;

	unique_ptr<Map> _map;
};

