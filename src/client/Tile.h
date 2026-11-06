#pragma once

struct Tile final
{
	using Type = tile::Type;

	Tile(Type type, const sf::Vector2i & pos);
	~Tile();

	void Render(sf::RenderTarget & tgt, const sf::RenderStates & states) const;

	Type _type;
	sf::Vector2i _pos;
	// The diamond: top, right, bottom, left (a fan of two triangles)
	sf::VertexBuffer _vertex_buffer{ sf::PrimitiveType::TriangleFan };
	sf::VertexArray _vertex_array;
};
