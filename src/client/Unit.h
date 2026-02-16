#pragma once

class Unit
{
public:
	virtual ~Unit();

	TPt _pt;

	bool IsMoving() const
	{
		// #todo
		return false;
	}

	unit::Action _action;
	Direction _dir;

	float _move_value;
};

