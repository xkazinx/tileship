#pragma once

class Unit;

class Units
{
public:

	vector<unique_ptr<Unit>> _units_uptr;
	vector<Unit*> _units;
};

