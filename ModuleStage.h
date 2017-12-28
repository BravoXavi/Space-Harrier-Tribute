#ifndef __MODULESTAGE_H__
#define __MODULESTAGE_H__

#include "Module.h"
#include "Font.h"
#include <list>

struct SDL_Texture;

class ModuleStage : public Module
{
public:
	ModuleStage(bool active = true);
	~ModuleStage();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	void PrintUI();
	void GenerateObstacles();

private:
	void EndingAndScoreBoard();

public:
	uint introVoiceSFX;
	Uint32 enemySpawnTimer = 0;
	Uint32 tickUpdate = 0;

	int debugTimer = 0;

	int obstacleTimer1 = 0;
	int obstacleTimer2 = 0;

private:
	Uint32 UITimer = 0;

	SDL_Texture* floor = nullptr;
	SDL_Texture* background = nullptr;
	SDL_Texture* backgroundFront = nullptr;
	SDL_Texture* gui = nullptr;

	SDL_Rect topScoreBanner;
	SDL_Rect actualScoreBanner;
	SDL_Rect liveIcon;

	float scoreBoardPosition;
	std::map< const char*, int > scores;

	const static int stageNumber = 1;
	const char* stageName = "MOOT";
	int topScore = 1000000;

	bool bossActive = false;
	bool Ending = false;
};

#endif // __MODULESTAGE_H__