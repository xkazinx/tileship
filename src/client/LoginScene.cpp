#include "stdafx.h"
#include "LoginScene.h"
#include "App.h"

LoginScene::LoginScene() :
	_circle(50.f, 50)
{
	_circle.setFillColor(sf::Color::Red);
}

LoginScene::~LoginScene() = default;

void LoginScene::Tick()
{
}

void LoginScene::ProcessUI()
{
	ImGui::SetNextWindowPos(ImVec2(g_res_x / 2.f - 150.f, g_res_y / 2.f - 80.f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300.f, 0.f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Tileship");
	ImGui::InputText("Username", _user, sizeof(_user));
	ImGui::InputText("Password", _pass, sizeof(_pass), ImGuiInputTextFlags_Password);

	const Auth auth(_user, _pass);
	if (ImGui::Button("Login"))
	{
		Out("Sent Login Auth{}", Format(auth));
		_sent_user = auth._user;
		_status = "Logging in...";
		g_app->Send(MainOp::Login, auth);
	}

	ImGui::SameLine();
	if (ImGui::Button("Create Account"))
	{
		Out("Sent Create Account Auth{}", Format(auth));
		_status = "Creating the account...";
		g_app->Send(MainOp::CreateAccount, auth);
	}

	if (!_status.empty())
		ImGui::TextWrapped("%s", _status.c_str());

	ImGui::End();
}

void LoginScene::Render()
{
	g_app->_window.draw(_circle);
}
