#include "stdafx.h"
#include "LoginScene.h"
#include "App.h"

LoginScene::LoginScene() : 
	_circle(50.f, 50)
{
	_name.resize(player::kNameSize);
}

LoginScene::~LoginScene()
{

}

void LoginScene::Tick()
{
}

void LoginScene::ProcessUI() const
{
	ImGui::SetNextWindowSize({ 250, 150 });
	ImGui::Begin("Authentication");
	ImGui::InputText("Username", (char*)_name.data(), _name.size());

	if (ImGui::Button("Login"))
	{
		if(g_sock)
		{
			auto pkt = g_app->DynamicPacket(player::kNameSize + 1, MainOp::Login);
			pkt->WriteFixed(_name, player::kNameSize);
			Send(pkt);
		}
	}
	
	ImGui::End();
}

void LoginScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{

}

void LoginScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{

}

