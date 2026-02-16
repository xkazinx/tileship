#include "stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Tile.h"
#include "App.h"
#include "Player.h"

GameScene::GameScene()
{
	_g = this;
	/*{
		//const string data = "10";
		const string data = "3012012012";
		vector<unique_ptr<Tile>> tiles;
		auto size = (tile::LineSize)(data[0] - '0');

		for(int i = 1; i < (int)data.size(); ++i)
		{
			int sep = ((i % 2) == 0) ? 0 : 3;
			tiles.emplace_back(make_unique<Tile>((tile::Type)(data[i] - '0'),
				sf::Vector2i{ ((i - 1) % size) + (int)sep, (int)floor((i - 1) / size) + (int)sep}));
		}

		_map = make_unique<Map>(tiles, size);
	}*/

	_map = make_unique<Map>();
}


GameScene::~GameScene()
{
	_p = nullptr;
}

void GameScene::Tick()
{
	_map->UpdateCamera(true);
}

void GameScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	_map->Render(tgt, states);
}

void GameScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{
	
}
