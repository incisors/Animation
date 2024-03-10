#pragma once

#include "KeyFrame.h"
#include <vector>
#include "Tokenizer.h"
#include <iostream>
#include <string>
class Channel
{
private:
	int extrap[2];
	int index = 0;
	float time_start = 0, time_end = 0;
	float tangent_start = 0, tangent_end = 0;
	float period;
	float offset;

	std::vector<Keyframe*> keyframes;

public:
	static const int CONSTANT, LINEAR, CYCLE;
	static const int  CYCLE_OFFSET, BOUNCE;
	static const int NOT_DEFINE;

	Channel(Tokenizer* fileToken);
	~Channel();

	void load(Tokenizer* fileToken);
	void precompute();
	float evaluate(float t);
	
};