#pragma once
#include <scene/manager.h>

struct Scene;
struct LoginScene;
struct GameScene;

// The client's scenes, each made once, on kazilib's scene manager (one current)
struct SceneMgr
{
	using ID = SceneID;

	SceneMgr();
	~SceneMgr();

	unique_ptr<LoginScene> _login;
	unique_ptr<GameScene> _game;
	lib::scene::Manager _mgr;
	ID _id = ID::Login;

	void Set(ID id);
	void Render();
	void Tick();
	void HandleEvent(const sf::Event & ev);
};
