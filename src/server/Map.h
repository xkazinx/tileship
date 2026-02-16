#pragma once

class Tile;
class Player;
class Unit;

class Map : public map::Base, public Broadcaster<MapOp>
{
public:
	using Op = MapOp;

	Map();
	~Map() override;

	void NetOperationProcess() override;

	vector<unique_ptr<Tile>> _tiles;

	Tile * Get(const TPt & pt)
	{
		return _tiles[pt.y + pt.x * _size.y].get();
	}

	template <typename ... Args>
	void Send(MapOp op, Args && ... args)
	{
		auto out = broadcaster::Base<Op>::StaticPacket(op, forward<Args>(args)...);
		for(auto player : _players_list)
			Global::Send(player, out);
	}

	array<Player*, player::kMaxGame> _players_arr;
	vector<Player*> _players_list;

	void ClearUnit(Unit * unit);
};


