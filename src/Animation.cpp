#include "Animation.h"

Animation::Animation(std::string _fileName)
{
	fileName = _fileName;
	load();
}

Animation::~Animation()
{
	for (auto &it : channels)
	{
		delete it;
	}
}

void Animation::load()
{
	std::cout << "Loading " + fileName << std::endl;

	Tokenizer *fileToken = new Tokenizer();
	fileToken->Open(fileName.c_str());

	char tokenBuff[20];
	fileToken->GetToken(tokenBuff);
	if (strcmp(tokenBuff, "animation") == 0)
	{
		fileToken->GetToken(tokenBuff);
		fileToken->GetToken(tokenBuff);
		if (strcmp(tokenBuff, "range") == 0)
		{
			time_start = fileToken->GetFloat();
			time_end = fileToken->GetFloat();

			int numOfChannel = 0;
			fileToken->GetToken(tokenBuff);
			if (strcmp(tokenBuff, "numchannels") == 0)
			{
				numOfChannel = fileToken->GetInt();
			}

			for (; numOfChannel > 0; numOfChannel--)
			{
				fileToken->GetToken(tokenBuff);
				if (strcmp(tokenBuff, "channel") != 0)
					continue;

				Channel *currChannel = new Channel(fileToken);
				channels.push_back(currChannel);
			}
		}
	}

	fileToken->Close();
	delete fileToken;
}

void Animation::update(float &t, glm::mat4 &T, Skeleton *skel)
{
	if (t < time_start || t > time_end)
		t = time_start;

	glm::vec3 pos;
	pos.x = channels[0]->evaluate(t);
	pos.y = channels[1]->evaluate(t);
	pos.z = channels[2]->evaluate(t);
	std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;

	T = glm::translate(pos);

	skel->root->offset = pos;
	// skel->root->update(T);
	// skel->update(T);
	int jointsNum = skel->joints.size();
	for (int i = 0; i < jointsNum; i++)
	{
		glm::vec3 rot;
		rot.x = channels[3 * i + 3]->evaluate(t);
		rot.y = channels[3 * i + 4]->evaluate(t);
		rot.z = channels[3 * i + 5]->evaluate(t);
		skel->joints[i]->setRotation(rot);
	}
}