#pragma once

#include "Mesh.h"

class Character
{
public:
	Character(char* fileName);
	~Character();

	Mesh* model;
};

