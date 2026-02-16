#include "stdafx.h"
#include "GameMgr.h"
#include "Game.h"
#include "Map.h"

Game* GameMgr::Get(game::ID id)
{
	for (auto& game : _list)
	{
		if (game->_id == id)
			return game.get();
	}

	return nullptr;
}

Game* GameMgr::GetOrCreate(game::ID id)
{
	if(auto game = Get(id))
		return game;

	_list.emplace_back(make_unique<Game>(id));
	return _list.back().get();
}
