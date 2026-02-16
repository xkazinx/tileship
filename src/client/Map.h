#pragma once

class Tile;

class Map
{
public:
	Map(vector<unique_ptr<Tile>>& tiles, tile::LineSize line_size);
	~Map();

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;

	vector<unique_ptr<Tile>> _tiles;
	tile::LineSize _line_size;

	// Resolution / kTileSz
	RPt _visible_tiles;
	// VisibleTiles / 2
	RPt _visible_tiles_range;
	// PlayerTilePoint - _visible_tiles_range
	RPt _visible_tile_offset;
	// WorldPoint of _visible_tile_offset * kTileSz
	RPt _world_offset;
	// _half_tile_offset + _world_offset
	RPt _world_offset_adjust;
	// Value that gets modified to direct movement
	MPt _world_offset_movement {0.f, 0.f};
	// -16 difference
	RPt _half_tile_offset = {0, 0};
};