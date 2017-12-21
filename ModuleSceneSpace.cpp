#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCollision.h"
#include "ModuleInput.h"
#include "ModuleParticles.h"
#include "ModuleSceneSpace.h"
#include "ModuleObstacle.h"
#include "ModuleEnemy.h"
#include "FontManager.h"

ModuleSceneSpace::ModuleSceneSpace(bool active) : Module(active)
{}

ModuleSceneSpace::~ModuleSceneSpace()
{}

// Load assets
bool ModuleSceneSpace::Start()
{
	LOG("Loading space scene");
	
	floor = App->textures->Load("assets/Floor.png");
	background = App->textures->Load("assets/backgroundlvl1.png");
	backgroundFront = App->textures->Load("assets/backgroundlvl2.png");
	gui = App->textures->Load("assets/GUI.png");

	actualScoreBanner.x = 1;
	actualScoreBanner.y = 0;
	actualScoreBanner.w = 48;
	actualScoreBanner.h = 14;

	topScoreBanner = { 9, 15, 32, 14 };
	liveIcon = { 20, 31, 10, 16 };

	startFx = App->audio->LoadFx("assets/initVoice.wav");

	App->player->Enable();
	App->particles->Enable();
	App->collision->Enable();

	//App->audio->PlayFx(startFx);
	//App->audio->PlayMusic("assets/Theme.wav", 1.0f);

	enemySpawnTimer = SDL_GetTicks();

	return true;
}

// UnLoad assets
bool ModuleSceneSpace::CleanUp()
{
	LOG("Unloading space scene");

 	App->textures->Unload(floor);
	App->player->Disable();
	App->collision->Disable();
	App->particles->Disable();

	return true;
}

//Draw Floor, Background, and extras (All this parts will ALWAYS stay in the back of the screen)
update_status ModuleSceneSpace::PreUpdate()
{
	App->renderer->BackgroundBlit(background, 0.2f, 1);
	App->renderer->BackgroundBlit(backgroundFront, 0.5f, 2);

	App->renderer->FloorBlit(floor, nullptr);
	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleSceneSpace::Update()
{
	// Draw everything --------------------------------------
		


	//if (timeCounter2 < 50) timeCounter2++;
	//else
	//{
	//	timeCounter2 = 0;
	//	int randNumX = rand() % (200 - (-200) + 1) + (-200);
	//	int randNumY = rand() % (170 - 80 + 1) + 80;

	//	App->obstacles->AddObstacle(App->obstacles->rock, ((float)SCREEN_WIDTH / 2.0f), (float)randNumX, (float)randNumY, WALL);
	//}

	//if (timeCounter2 < 1)
	//{
	//	//App->obstacles->AddObstacle(App->obstacles->bush, ((float)SCREEN_WIDTH / 2.0f), 0.0f, 0.0f, NOLETHAL_D_OBSTACLE);
	//	//App->enemies->AddEnemy(App->enemies->alienShip, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, 15.0f, ENEMY, 1);
	//	//App->enemies->AddEnemy(App->enemies->tomos, (2.0f*M_PI)/3.0f, 0.0f, 20.0f, ENEMY, 1);
	//	//App->enemies->AddEnemy(App->enemies->tomos, (4.0f*M_PI)/3.0f, 0.0f, 20.0f, ENEMY, 1);
	//	timeCounter2++;
	//}

	//------------------------------------------------------------------------------

	tickUpdate = SDL_GetTicks();

	actualScore += 0.2;
	if ((int)actualScore > topScore) topScore = actualScore;

	if (App->enemies->triggerEnemies)
	{
		if (!App->enemies->bossEncounter)
		{
			if (tickUpdate - enemySpawnTimer > 300.0f)
			{
				enemySpawnTimer = tickUpdate;
				App->enemies->enemyWave(App->enemies->waveNum);
			}
		}
	}
	else
	{
		if (tickUpdate - enemySpawnTimer > 6000.0f && !App->enemies->aliveEnemy)
		{
			App->enemies->triggerEnemies = true;
			App->enemies->waveNum++;

			LOG("NEXT WAVE -> Wave %i -------------------", App->enemies->waveNum);
		}
	}

	GenerateObstacles();
	PrintUI();

	return UPDATE_CONTINUE;
}

void ModuleSceneSpace::PrintUI()
{
	string stage = "STAGE " + to_string(stageNumber);
	int charWidth = App->fontManager->blueFont->characterWidth;
	int charHeight = App->fontManager->blueFont->characterHeight;

	if (App->enemies->waveNum == 0 && (tickUpdate - enemySpawnTimer < 3000.0f))
	{
		App->fontManager->blueFont->printText(stage.c_str(), (SCREEN_WIDTH/2) - (stage.length()*charWidth/2), SCREEN_HEIGHT/2);
		App->fontManager->blueFont->printText(stageName, (SCREEN_WIDTH / 2) - (strlen(stageName)*charWidth/2), SCREEN_HEIGHT/2 + charHeight + 2);
	}

	//Stage number
	App->fontManager->blueFont->printText(stage.c_str(), SCREEN_WIDTH - stage.length()*charWidth, SCREEN_HEIGHT - charHeight, 0.8f);

	//Top Score info
	int xPos = 0;
	int resizedWidth = topScoreBanner.w / 1.5;
	int resizedHeight = topScoreBanner.h / 1.5;
	App->renderer->Blit(gui, xPos, resizedHeight/2, &topScoreBanner, topScoreBanner.w / 1.5, topScoreBanner.h / 1.5);

	xPos += resizedWidth + 4;
	App->fontManager->redFont->printText(to_string(topScore).c_str(), xPos, 6, 0.8f);

	//Actual Score info
	resizedWidth = actualScoreBanner.w / 1.5;
	resizedHeight = actualScoreBanner.h / 1.5;
	xPos = 2.5*SCREEN_WIDTH/4;
	App->renderer->Blit(gui, xPos, resizedHeight / 2, &actualScoreBanner, actualScoreBanner.w / 1.5, actualScoreBanner.h / 1.5);

	xPos += resizedWidth + 4;
	App->fontManager->greenFont->printText(to_string((int)actualScore).c_str(), xPos, 6, 0.8f);

	//Lives info
	resizedWidth = liveIcon.w / 2;
	resizedHeight = liveIcon.h / 2;
	for (int i = 0; i < App->player->lives; i++)
	{
		App->renderer->Blit(gui, 4 + (i * resizedWidth), SCREEN_HEIGHT - resizedHeight - 2, &liveIcon, resizedWidth, resizedHeight);
	}
}

void ModuleSceneSpace::GenerateObstacles()
{
	int randX, randY = 0;

	if (obstacleTimer1 < 20) obstacleTimer1++;
	else
	{
		obstacleTimer1 = 0;
		randX = rand() % (350 - (-350) + 1) + (-350);
		
		App->obstacles->AddObstacle(App->obstacles->bush, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, NOLETHAL_D_OBSTACLE);
	}

	if (obstacleTimer2 < 50) obstacleTimer2++;
	else
	{
		obstacleTimer2 = 0;
		randX = rand() % (350 - (-350) + 1) + (-350);
		randY = rand() % (150 - 80 + 1) + 80;
		
		if( App->enemies->waveNum < 4) App->obstacles->AddObstacle(App->obstacles->rock, ((float)SCREEN_WIDTH / 2.0f), (float)randX, (float)randY, D_OBSTACLE);
		else App->obstacles->AddObstacle(App->obstacles->tree, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, D_OBSTACLE);
	}	
}