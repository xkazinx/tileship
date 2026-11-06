#pragma once

struct Tile;

struct Map
{
	Map(vector<unique_ptr<Tile>> & tiles, tile::LineSize line_size);
	~Map();

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states) const;

	tile::LineSize _line_size;
	vector<unique_ptr<Tile>> _tiles;
};
