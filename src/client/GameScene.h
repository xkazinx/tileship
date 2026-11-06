#pragma once
#include "Scene.h"

struct Map;
struct Tile;

struct GameScene : Scene
{
	GameScene();
	~GameScene() override;

	static constexpr auto kID = SceneID::Game;

	// The map, made the first time the scene is entered (its vertex buffers need
	// the window's GL context)
	void Initialize() override;
	void Tick() override;
	void Render() override;
	void ProcessUI() override;

	unique_ptr<Map> _map;
};
