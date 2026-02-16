#pragma once

class Game;

class GameMgr
{
public:
	vector<unique_ptr<Game>> _list;

	Game * Get(game::ID id);

	Game * GetOrCreate(game::ID id);
};

