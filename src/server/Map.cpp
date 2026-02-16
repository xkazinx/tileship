#include "stdafx.h"
#include "Map.h"
#include "Tile.h"
#include "Game.h"
#include "Unit.h"

Map::Map() : map::Base({ 100, 100 }), Broadcaster<MapOp>(NetOp::Map)
{
	TPt pos;
	for(pos.x = 0u; pos.x < _size.x; ++pos.x)
		for(pos.y = 0u; pos.y < _size.y; ++pos.y)
			_tiles.emplace_back(make_unique<Tile>(tile::Type::Grass, pos));	
}

Map::~Map()
{

}

void Map::NetOperationProcess()
{
	switch(GetOperation())
	{
	
	}
}

void Map::ClearUnit(Unit* unit)
{
	if (auto tile = Get(unit->_pt))
	{
		tile->_unit = nullptr;
	}
}
