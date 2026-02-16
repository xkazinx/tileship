#pragma once
#include "Unit.h"

class Player : public Unit
{
public:
	Player(const string & name);
	~Player() override;

	string _name;
};

