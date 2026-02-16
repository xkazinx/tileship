#include "stdafx.h"
#include "Game.h"
#include "Map.h"
#include "Player.h"
#include "Tile.h"

Game::Game(game::ID id) : _id(id)
{
	_map = make_unique<Map>();
}

void Game::EnterGame(Player * player)
{
	TPt pt;
	for(pt.x = 0; pt.x < _map->_size.x; ++pt.x)
		for(pt.y = 0; pt.y < _map->_size.y; ++pt.y)
		{
			if(auto tile = _map->Get(pt))
			{
				player->_pt = pt;
				tile->_unit = player;

				for(player::GameIdx i = 0; i < _map->_players_arr.size(); ++i)
				{
					if(!_map->_players_arr[i])
					{
						_map->_players_arr[i] = player;
						player->_game_idx = i;
						break;
					}
				}

				_map->_players_list.push_back(player);

				{
					auto out = _map->DynamicPacket((player::kNameSize + sizeof(u32) + sizeof(TPt) + sizeof(PlayerID)) * player::kMaxGame, MapOp::SelfSpawn);
					for(auto & p : _map->_players_arr)
					{
						out->Write(p ? p->_id : player::kNullID);
						if(p)
						{
							out->Write(p->_pt, p->_name);
						}
					}
					Print("MapOp::SelfSpawn");
					Global::Send(player, out);
				}

				_map->Send(MapOp::UnitSpawn, player->_game_idx, player->_pt, player->_id, player->_name);
				return;
			}
		}
}

void Game::ExitGame(Player * player)
{
	for(auto & p : _map->_players_arr)
	{
		if(p == player)
		{
			p = nullptr;
		}
	}

	_map->_players_list.erase(std::remove(_map->_players_list.begin(), _map->_players_list.end(), player), _map->_players_list.end());

	_map->ClearUnit(player);
}