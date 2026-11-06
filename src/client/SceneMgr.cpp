#include "stdafx.h"
#include "SceneMgr.h"
#include "LoginScene.h"
#include "GameScene.h"

SceneMgr::SceneMgr()
	: _login(make_unique<LoginScene>()), _game(make_unique<GameScene>())
{
}

SceneMgr::~SceneMgr() = default;

void SceneMgr::Set(const ID id)
{
	using enum ID;
	switch (id)
	{
	case Login:	_mgr.Set(*_login);	break;
	case Main:						return;
	case Lobby:						return;
	case Game:	_mgr.Set(*_game);	break;
	case Result:					return;
	default:						return;
	}
	_id = id;
}

void SceneMgr::Render()
{
	_mgr.Render();
	if (auto * const s = static_cast<Scene *>(_mgr._current))
		s->ProcessUI();
}

void SceneMgr::Tick()
{
	_mgr.Tick();
}

void SceneMgr::HandleEvent(const sf::Event & ev)
{
	_mgr.HandleEvent(&ev);
}
