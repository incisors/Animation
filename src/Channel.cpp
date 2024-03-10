#include "Channel.h"

const int Channel::NOT_DEFINE = 0;
const int Channel::CONSTANT = 1;
const int Channel::LINEAR = 2;
const int Channel::CYCLE = 3;
const int Channel::CYCLE_OFFSET = 4;
const int Channel::BOUNCE = 5;

Channel::Channel(Tokenizer* fileToken)
{
	load(fileToken);
	precompute();

	if (keyframes.size() == 0)
	{
		std::cout << "No keyframe in channel" << std::endl;
	}
	else
	{
		time_start = keyframes.front()->getTime();
		time_end = keyframes.back()->getTime();
		tangent_start = keyframes.front()->getTangentIn();
		tangent_end = keyframes.back()->getTangentOut();
		period = time_end - time_start;
		offset = keyframes.back()->getValue() - keyframes.front()->getValue();
	}
}

Channel::~Channel()
{
	for (auto& key : keyframes)
	{
		delete key;
	}
}

void Channel::load(Tokenizer* fileToken)
{
	char tokenBuff[20];
	index = fileToken->GetInt();
	fileToken->GetToken(tokenBuff); // "{"
	fileToken->GetToken(tokenBuff);
	if (strcmp(tokenBuff, "extrapolate") == 0)
	{
		for (int i = 0; i < 2; i++)
		{
			fileToken->GetToken(tokenBuff);
			if (strcmp(tokenBuff, "constant") == 0)
			{
				extrap[i] = CONSTANT;
			}
			else if (strcmp(tokenBuff, "linear") == 0)
			{
				extrap[i] = LINEAR;
			}
			else if (strcmp(tokenBuff, "cycle") == 0)
			{
				extrap[i] = CYCLE;
			}
			else if (strcmp(tokenBuff, "cycle_offset") == 0)
			{
				extrap[i] = CYCLE_OFFSET;
			}
			else if (strcmp(tokenBuff, "bounce") == 0)
			{
				extrap[i] = BOUNCE;
			}
			else
			{
				extrap[i] = NOT_DEFINE;
			}
		}

		fileToken->GetToken(tokenBuff);
		if (strcmp(tokenBuff, "keys") == 0)
		{
			int numOfKey = fileToken->GetInt();
			fileToken->GetToken(tokenBuff);

			for (; numOfKey > 0; numOfKey--)
			{
				Keyframe* currKey = new Keyframe(fileToken);
				keyframes.push_back(currKey);
			}

			fileToken->GetToken(tokenBuff);
		}
	}

	fileToken->GetToken(tokenBuff);
}

void Channel::precompute()
{
	int numOfKeys = keyframes.size();
	if (numOfKeys == 1)
	{
		keyframes[0]->iniTangent(nullptr, nullptr);
		keyframes[0]->iniCubic(nullptr);
	}
	else if (numOfKeys == 2)
	{
		keyframes[0]->iniTangent(nullptr, keyframes[1]);
		keyframes[1]->iniTangent(keyframes[0], nullptr);
		keyframes[0]->iniCubic(keyframes[1]);
	}
	else
	{
		keyframes.front()->iniTangent(nullptr, keyframes[1]);
		for (int i = 1; i < numOfKeys - 1; i++)
		{
			keyframes[i]->iniTangent(keyframes[i - 1], keyframes[i + 1]);
		}
		keyframes.back()->iniTangent(keyframes[numOfKeys - 2], nullptr);

		for (int i = 0; i < numOfKeys - 1; i++)
		{
			keyframes[i]->iniCubic(keyframes[i + 1]);
		}
	}
}

float Channel::evaluate(float t)
{
	float value_offset = 0.0f;

	if (t <= time_start)
	{
		if (extrap[0] == CONSTANT || extrap[0] == NOT_DEFINE)
		{
			return keyframes.front()->getValue();
		}
		else if (extrap[0] == LINEAR)
		{
			return tangent_start * (t - time_start) + keyframes.front()->getValue();
		}
		else if (extrap[0] == CYCLE)
		{
			int count = (time_end - t) / period;
			t += count * period;
		}
		else if (extrap[0] == CYCLE_OFFSET)
		{
			int count = (time_end - t) / period;
			t += count * period;
			value_offset = -offset * count;
		}
		else if (extrap[0] == BOUNCE)
		{
			int count = (time_end - t) / period;
			t += count * period;
			if (count % 2 == 1)
			{
				t = time_start + time_end - t;
			}
		}
	}
	else if (t >= time_end)
	{
		if (extrap[1] == CONSTANT || extrap[1] == NOT_DEFINE)
		{
			return keyframes.back()->getValue();
		}
		else if (extrap[1] == LINEAR)
		{
			return tangent_end * (t - time_end) + keyframes.back()->getValue();
		}
		else if (extrap[1] == CYCLE)
		{
			int count = (t - time_start) / period;
			t -= count * period;
		}
		else if (extrap[1] == CYCLE_OFFSET)
		{
			int count = (t - time_start) / period;
			t -= count * period;
			value_offset = offset * count;
		}
		else if (extrap[1] == BOUNCE)
		{
			int count = (t - time_start) / period;
			t -= count * period;
			if (count % 2 == 1)
			{
				t = time_start + time_end - t;
			}
		}
	}

	std::vector<Keyframe*>::iterator it;
	it = std::lower_bound(keyframes.begin(), keyframes.end(), t, [&](Keyframe* key, float t)->bool {
			// std::cout << "key: " << key->getTime() << ", t: " << t << std::endl;
			return key->getTime() < t;
		}
	);
	if(it == keyframes.begin()) {
		return (*it)->evaluate(0.0f) + value_offset;
	}
	it--;
	return (*it)->evaluate(t) + value_offset;
}