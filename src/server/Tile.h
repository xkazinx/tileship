#pragma once

class Unit;

class Tile
{
public:
	Tile(tile::Type type, const TPt& pos)
	: _type(type), _pos(pos) {}
	~Tile(){}

	tile::Type _type;
	TPt _pos;

	Unit * _unit;
};


