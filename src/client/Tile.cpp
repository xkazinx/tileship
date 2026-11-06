#include "stdafx.h"
#include "Tile.h"
#include "Map.h"

Tile::Tile(const Type type, const sf::Vector2i & pos)
	: _type(type), _pos(pos)
{
	if (!_vertex_buffer.create(4))
		Err("Tile: no vertex buffer");

	constexpr auto half = tile::kHalfSize;
	constexpr auto origin = 300.f;
	const sf::Color color = tile::kColors[to_underlying(type)];
	const float x = origin + ((pos.x - pos.y) * half);
	const float y = origin + ((pos.x + pos.y) * half);

	_vertex_array.append(sf::Vertex{ { x, y }, color });
	_vertex_array.append(sf::Vertex{ { x + half, y + half }, color });
	_vertex_array.append(sf::Vertex{ { x, y + tile::kSize }, color });
	_vertex_array.append(sf::Vertex{ { x - half, y + half }, color });

	if (!_vertex_buffer.update(&_vertex_array[0]))
		Err("Tile: vertex buffer update failed");
}

Tile::~Tile() = default;

void Tile::Render(sf::RenderTarget & tgt, const sf::RenderStates & states) const
{
	tgt.draw(_vertex_buffer, states);
}
