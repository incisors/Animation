#pragma once

#include "Animation.h"
#include "Skeleton.h"
#include <ctime>

class Player
{
private:
	Skeleton* skel;
	Animation* anim;

	clock_t prevT;
	float time;

	std::vector<Joint*> joints;

public:

	Player(Skeleton* _skel, Animation* _anim);

	void update();
};