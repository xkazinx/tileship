#pragma once
#include <scene/manager.h>

// A client scene (kazilib scene/manager.h): Initialize when entered, Render each
// frame on the window, HandleEvent with an sf::Event, and its ImGui windows
struct Scene : lib::scene::Base
{
	// Its ImGui windows, each frame after Render
	virtual void ProcessUI() {}
	// One window event
	virtual void HandleEvents([[maybe_unused]] const sf::Event & ev) {}

	void HandleEvent(const void * const event) override
	{
		if (event)
			HandleEvents(*static_cast<const sf::Event *>(event));
	}
};
