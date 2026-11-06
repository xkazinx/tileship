#include "stdafx.h"
#include "Map.h"
#include "Tile.h"

Map::Map(vector<unique_ptr<Tile>> & tiles, const tile::LineSize line_size)
	: _line_size(line_size), _tiles(std::move(tiles))
{
	g_map = this;
}

Map::~Map()
{
	if (g_map == this)
		g_map = nullptr;
}

void Map::Render(sf::RenderTarget & tgt, const sf::RenderStates & states) const
{
	for (const auto & tile : _tiles)
	{
		tile->Render(tgt, states);
	}
}
