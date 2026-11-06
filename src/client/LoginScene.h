#pragma once
#include "Scene.h"

struct LoginScene : Scene
{
	LoginScene();
	~LoginScene() override;

	static constexpr auto kID = SceneID::Login;

	void Tick() override;
	void Render() override;
	void ProcessUI() override;

	sf::CircleShape _circle;

	// The ImGui fields (a name / password and its zero)
	char _user[player::kNameSize + 1]{};
	char _pass[player::kPasswordSize + 1]{};
	// The name of the last login sent (the account once the server accepts it)
	string _sent_user;
	// What the server last said (connection, login, new account)
	string _status = "Connecting...";
};
