#include "stdafx.h"

namespace common
{
	unique_ptr<Random> g_rnd = make_unique<Random>();

namespace tile
{
#ifdef CLIENT
	ColorArray kColors =
	{
		sf::Color::Green,
		sf::Color::Blue,
		sf::Color{ 50, 50, 50, 255 }
	};
#endif
}
}