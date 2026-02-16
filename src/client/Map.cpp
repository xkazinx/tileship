#include "Map.h"
#include "stdafx.h"
#include "Map.h"
#include "Tile.h"
#include "Player.h" 
#include "GameScene.h"

Map::Map() : map::Base({ 100, 100 }), Broadcaster<MapOp>(NetOp::Map)
{
	g_map = this;
	
	sf::Vector2i pos;
	for(pos.x = 0u; pos.x < _size.x; ++pos.x)
		for(pos.y = 0u; pos.y < _size.y; ++pos.y)
			_tiles.emplace_back(make_unique<Tile>(tile::Type::Grass, pos));

	{
		
		TPt g_visible_tiles(g_res.x / tile::kSize, g_res.y / tile::kSize);
		TPt g_visible_tiles_range(g_visible_tiles.x / 2, g_visible_tiles.y / 2);
		_visible_tiles       = g_visible_tiles;
		_visible_tiles_range = g_visible_tiles_range;

		if((_visible_tiles.x % 2) == 0)
			_half_tile_offset.x = -tile::kHalfSize;

		if((_visible_tiles.y % 2) == 0)
			_half_tile_offset.y = -tile::kHalfSize;
	}

	auto tile_cnt = static_cast<szt>(((g_res.x + 4 * kTileSz) / kTileSz) *
		((g_res.y + 4 * kTileSz) / kTileSz)
		+ (ceil(g_res.x / kTileSz) +
			ceil(g_res.y / kTileSz) + 1));

	grid.create(tile_cnt * 8);
	grid.setPrimitiveType(sf::PrimitiveType::Lines);
	grid.setUsage(sf::VertexBuffer::Dynamic);
	grid_vertex.resize(tile_cnt * 8);
}

Map::~Map()
{

}

void Map::NetOperationProcess()
{
	switch(GetOperation())
	{
		OP(UnitSpawn);
		OP(SelfSpawn);
	}
}

void Map::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	
	auto draw = [&](bool update_grid)
	{
		if(update_grid)
			grid.update(grid_vertex.data());

		tgt.draw(grid, 0, grid.getVertexCount(), states);
	};

	u32 i;

	constexpr auto extra_tile_sz = tile::kSize + tile::kHalfSize;

	RPt rpt;
	TPt tpt;

	for(i     = 0,
			 tpt.y = _visible_tile_offset.y - 1,
			 rpt.y = -tile::kSize + (_world_offset_adjust.y - _world_offset.y) + RVal(_world_offset_movement.y);
			 rpt.y <= g_res.y + extra_tile_sz;
			 rpt.y += tile::kSize, ++tpt.y)
		{
			for(tpt.x = _visible_tile_offset.x - 1,
				 rpt.x = -tile::kSize + (_world_offset_adjust.x - _world_offset.x) + RVal(_world_offset_movement.x);
				 rpt.x <= g_res.x + extra_tile_sz;
				 rpt.x += tile::kSize, ++tpt.x, ++i)
			{
				//GetSprite(tpt)->rTile(rpt, GetData(tpt).GetFrame());
				RenderTilesAndObjects(tpt, rpt, i, tgt, states);
			}
		}

	draw(true);
}

void Map::RenderTilesAndObjects(TPt tpt, RPt rpt, u32 i, sf::RenderTarget & target, sf::RenderStates states) const
{
	static sf::Vertex point;

	constexpr auto kShowPoint = false;
	if(kShowPoint)
	{
		//tout::Fixed(rpt, format("{}", rpt.x), sf::Color::White);
		//tout::Fixed(rpt + RPt(0, 15), format("{}", rpt.x), sf::Color::White);
	}

	//const bool display_grid = _g->_grid;
	const bool display_grid = true;
	if(display_grid && i * 8 < grid_vertex.size())
	{
		auto v  = &grid_vertex[i * 8];
		const auto px = static_cast<float>(rpt.x);
		const auto py = static_cast<float>(rpt.y);

		if(v[0].position != sf::Vector2f(px, py))
		{
			v[0].position = sf::Vector2f(px, py);
			v[1].position = sf::Vector2f(px + kTileSz, py);
			v[2].position = sf::Vector2f(px + kTileSz, py);
			v[3].position = sf::Vector2f(px + kTileSz, py + kTileSz);
			v[4].position = sf::Vector2f(px + kTileSz, py + kTileSz);
			v[5].position = sf::Vector2f(px, py + kTileSz);
			v[6].position = sf::Vector2f(px, py + kTileSz);
			v[7].position = sf::Vector2f(px, py);

			auto color = sf::Color::Green;
			color.a = 255/2;

			if(auto tile = Get(tpt);
				!tile)
			{
			Print("tile: {} {}", tpt.x, tpt.y);
				color = sf::Color::Transparent;
			}

			if(v[0].color != color)
			{
				v[0].color = color;
				v[1].color = color;
				v[2].color = color;
				v[3].color = color;
				v[4].color = color;
				v[5].color = color;
				v[6].color = color;
				v[7].color = color;
			}
		}
	}
}


void Map::UpdateCamera(bool calc_all)
{
	_visible_tile_offset.x = _p->_pt.x - _visible_tiles_range.x;
	_visible_tile_offset.y = _p->_pt.y - _visible_tiles_range.y;

	_world_offset.x = _visible_tile_offset.x * tile::kSize;
	_world_offset.y = _visible_tile_offset.y * tile::kSize;

	_world_offset_adjust.x = _half_tile_offset.x + _world_offset.x;
	_world_offset_adjust.y = _half_tile_offset.y + _world_offset.y;
	
	Print("_p->_pt {} {}, _visible_tile_offset {} {}", _p->_pt.x, _p->_pt.y, _visible_tile_offset.x, _visible_tile_offset.y);
	if(!calc_all)
		return;

	{
		_world_offset_movement = { 0, 0 };

		if(_p->IsMoving())
		{
			if(_p->_action == unit::Action::Move)
			{
				_world_offset_movement.x += dir::GetDelta(_p->_dir).x * tile::kSize;
				_world_offset_movement.y += dir::GetDelta(_p->_dir).y * tile::kSize;
			}

			_world_offset_movement.x += ((round(_p->_move_value)) * ((dir::GetDelta(dir::GetContrary(_p->_dir)).x)));
			_world_offset_movement.y += ((round(_p->_move_value)) * (dir::GetDelta(dir::GetContrary(_p->_dir)).y));
		}
	}

	//_g->AdjustViewPoint();
}

void Map::NetOp_UnitSpawn()
{
	auto game_idx = GetPacket()->ReadAs<player::GameIdx>();

	auto & p = _g->_players[game_idx];
	if(!p)
	{
		p = make_unique<Player>();
		p->_game_idx = game_idx;
		GetPacket()->Read(p->_pt, p->_id, p->_name);
	}

	SetUnit(p.get());

	Print("Unit spawn {}", p->_name);
}

void Map::SetUnit(Unit * unit)
{
	// #todo check pos
	auto tile = Get(unit->_pt);
	if(!tile)
		return;

	if(tile->_unit)
	{
		Log::Error("Trying to set unit in already set tile");
		return;
	}

	tile->_unit = unit;
}

void Map::NetOp_SelfSpawn()
{
	TPt pt;
	PlayerID id;
	string name;

	for(player::GameIdx i = 0; i < _g->_players.size(); ++i)
	{
		auto id = GetPacket()->ReadAs<PlayerID>();
		if(id == player::kNullID)
			continue;
		
		auto & p = _g->_players[i];
		p = make_unique<Player>();
		p->_game_idx = i;
		p->_id = id;
		GetPacket()->Read(p->_pt, p->_name);

		if(p->_name == Player::LoginName)
		{
			_p = p.get();
		}
	}

	Print("Received player spawn data");
}