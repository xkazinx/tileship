#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"

PlayerMgr::PlayerMgr()
{

}

PlayerMgr::~PlayerMgr()
{
	
}

Player* PlayerMgr::OnAuth(ENetPeer * peer)
{
	Player* player = nullptr;

	if (auto& player_idx = reinterpret_cast<PlayerIdx*&>(peer->data); 
		!player_idx)
	{
		for (PlayerIdx i = 1; i < player::kMax; i++)
		{
			if (auto& p = g_player_mgr->_array[i];
				!p)
			{
				player_idx = new PlayerIdx(i);
				p = make_unique<Player>(i, peer);
				player = p.get();
				_list.push_back(p.get());
				
				Print("OnAuth: PlayerIdx {}", i);
				break;
			}
		}
	}
	else
	{
		player = g_player_mgr->_array[*player_idx].get();
	}

	return player;
}