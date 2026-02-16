#pragma once

class Tile;
class Unit;

class Map : public map::Base, public Broadcaster<MapOp>
{
public:
	using Op = MapOp;

	Map();
	~Map();

	void NetOperationProcess() override;

	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;

	void RenderTilesAndObjects(TPt tpt, RPt rpt, u32 i, sf::RenderTarget& target, sf::RenderStates states) const;

	void UpdateCamera(bool calc_all);

	void NetOp_UnitSpawn();
	void NetOp_SelfSpawn();

	mutable vector<sf::Vertex> grid_vertex;
	mutable sf::RenderTexture  grid_texture;
	mutable sf::VertexBuffer   grid;
	
	mutable sf::RenderTexture  _bg_texture;
	bool _to_texture = true;

	vector<unique_ptr<Tile>> _tiles;
	Tile * Get(const TPt & pt)
	{
		if(pt.x <= -1 || pt.y <= -1 || _tiles.size() < pt.y + pt.x * _size.y)
			return nullptr;

		return _tiles[pt.y + pt.x * _size.y].get();
	}
	const Tile * Get(const TPt & pt) const
	{
		if (pt.x <= -1 || pt.y <= -1 || _tiles.size() < pt.y + pt.x * _size.y)
			return nullptr;

		return _tiles[pt.y + pt.x * _size.y].get();
	}
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

	void SetUnit(Unit * unit);
};