#include "stdafx.h"
#include "GameScene.h"
#include "App.h"
#include "Map.h"
#include "Tile.h"

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::Initialize()
{
	if (!InitOnce())
		return;

	//const string data = "10";
	const string data = "3012012012";
	vector<unique_ptr<Tile>> tiles;
	auto size = (tile::LineSize)(data[0] - '0');

	for (int i = 1; i < (int)data.size(); ++i)
	{
		int sep = ((i % 2) == 0) ? 0 : 3;
		tiles.emplace_back(make_unique<Tile>((tile::Type)(data[i] - '0'),
			sf::Vector2i{ ((i - 1) % size) + sep, (int)std::floor((i - 1) / size) + sep }));
	}

	_map = make_unique<Map>(tiles, size);
}

void GameScene::Tick()
{
}

void GameScene::Render()
{
	if (_map)
		_map->Render(g_app->_window, sf::RenderStates::Default);
}

void GameScene::ProcessUI()
{
	ImGui::SetNextWindowPos(ImVec2(10.f, 10.f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Player", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Logged in as %s", g_app->_user.c_str());
	ImGui::End();
}
