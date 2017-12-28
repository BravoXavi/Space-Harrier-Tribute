#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCollision.h"
#include "ModuleInput.h"
#include "ModuleParticles.h"
#include "ModuleStage.h"
#include "ModuleObstacle.h"
#include "ModuleShadows.h"
#include "ModuleFadeToBlack.h"
#include "ModuleEnemy.h"
#include "FontManager.h"

ModuleStage::ModuleStage(bool active) : Module(active)
{
	scores["ZELLERYON"] = 999999999;
	scores["NEIKAR"] = 800000000;
	scores["PSYHOLIC"] = 700000000;
	scores["DOGJOE"] = 500000000;
}

ModuleStage::~ModuleStage()
{}

// Load assets
bool ModuleStage::Start()
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

	introVoiceSFX = App->audio->LoadFx("assets/sfx/VOICE_Welcome.wav");

	App->player->Enable();
	App->particles->Enable();
	App->collision->Enable();
	App->enemies->Enable();
	App->obstacles->Enable();
	App->shadows->Enable();

	App->audio->PlayFx(introVoiceSFX);
	App->audio->PlayMusic("assets/music/MOOT_Theme.wav", 1.0f);

	enemySpawnTimer = SDL_GetTicks();
	scoreBoardPosition = (float)SCREEN_HEIGHT;
	return true;
}

// UnLoad assets
bool ModuleStage::CleanUp()
{
	LOG("Unloading space scene");

 	App->textures->Unload(floor);
	App->textures->Unload(background);
	App->textures->Unload(backgroundFront);
	App->textures->Unload(gui);
	App->player->Disable();
	App->collision->Disable();
	App->particles->Disable();
	App->enemies->Disable();
	App->obstacles->Disable();

	return true;
}

//Draw Floor, Background, and extras (All this parts will ALWAYS stay in the back of the screen)
update_status ModuleStage::PreUpdate()
{
	App->renderer->BackgroundBlit(background, 0.2f, 1);
	App->renderer->BackgroundBlit(backgroundFront, 0.5f, 2);

	App->renderer->FloorBlit(floor, nullptr);
	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleStage::Update()
{
	PrintUI();
	tickUpdate = SDL_GetTicks();

	if ((int)App->player->playerScore > topScore) topScore = (int)App->player->playerScore;

	if (App->player->lives > 0)
	{
		if (!App->enemies->bossEncounter)
		{
			App->player->playerScore += 0.3f;

			if (App->enemies->triggerEnemies)
			{
				if (tickUpdate - enemySpawnTimer > 300.0f)
				{
					enemySpawnTimer = tickUpdate;
					App->enemies->enemyWave(App->enemies->waveNum);
					if (App->enemies->waveNum == 8) App->enemies->bossEncounter = true;
				}
			}
			else
			{
				if (tickUpdate - enemySpawnTimer > 6000.0f && !App->enemies->aliveEnemy)
				{
					App->enemies->triggerEnemies = true;
					App->enemies->waveNum++;
					if(App->enemies->waveNum == 8) App->audio->PlayMusic("assets/music/MOOT_BossTheme.wav", 0.0f);
				}
			}
		}
		else if (App->enemies->bossEncounter && !App->enemies->aliveEnemy)
		{
			Uint32 scoreBoardTimer = SDL_GetTicks();
			if (tickUpdate - scoreBoardTimer > 2000.0f)
			{
				EndingAndScoreBoard();
			}
		}
	}
	else
	{
		EndingAndScoreBoard();
	}

	if(!App->player->gotHit) GenerateObstacles();

	return UPDATE_CONTINUE;
}

void ModuleStage::PrintUI()
{
	string stage = "STAGE " + to_string(stageNumber);
	int charWidth = App->fontManager->blueFont->characterWidth;
	int charHeight = App->fontManager->blueFont->characterHeight;

	if (App->enemies->waveNum == 0 && (tickUpdate - enemySpawnTimer < 3000.0f))
	{
		App->fontManager->blueFont->printText(stage.c_str(), ((float)SCREEN_WIDTH / 2.0f) - (stage.length()*charWidth/2.0f), (float)SCREEN_HEIGHT/2.0f);
		App->fontManager->blueFont->printText(stageName, ((float)SCREEN_WIDTH / 2.0f) - (strlen(stageName)*charWidth/2.0f), (float)SCREEN_HEIGHT/2.0f + (float)charHeight + 2.0f);
	}

	//Stage number
	App->fontManager->blueFont->printText(stage.c_str(), (float)(SCREEN_WIDTH - stage.length()*charWidth), (float)(SCREEN_HEIGHT - charHeight), 0.8f);

	//Top Score info
	int xPos = 0;
	int resizedWidth = (int)(topScoreBanner.w / 1.5f);
	int resizedHeight = (int)(topScoreBanner.h / 1.5f);
	App->renderer->Blit(gui, (float)xPos, (float)(resizedHeight/2), &topScoreBanner, (float)(topScoreBanner.w / 1.5f), (float)(topScoreBanner.h / 1.5f));

	xPos += resizedWidth + 4;
	App->fontManager->redFont->printText(to_string(topScore).c_str(), (float)xPos, 6.0f, 0.8f);

	//Actual Score info
	resizedWidth = (int)(actualScoreBanner.w / 1.5f);
	resizedHeight = (int)(actualScoreBanner.h / 1.5f);
	xPos = (int)(2.5*SCREEN_WIDTH/4);
	App->renderer->Blit(gui, (float)xPos, (float)(resizedHeight / 2), &actualScoreBanner, (float)(actualScoreBanner.w / 1.5f), (float)(actualScoreBanner.h / 1.5f));

	xPos += resizedWidth + 4;
	App->fontManager->greenFont->printText(to_string((int)App->player->playerScore).c_str(), (float)xPos, 6.0f, 0.8f);

	//Lives info
	resizedWidth = liveIcon.w / 2;
	resizedHeight = liveIcon.h / 2;
	for (int i = 0; i < App->player->lives; i++)
	{
		App->renderer->Blit(gui, (float)(4 + (i * resizedWidth)), (float)(SCREEN_HEIGHT - resizedHeight - 2), &liveIcon, (float)resizedWidth, (float)resizedHeight);
	}
}

void ModuleStage::GenerateObstacles()
{
	int randX, randY = 0;

	if (!bossActive)
	{
		if (obstacleTimer1 < 15) obstacleTimer1++;
		else
		{
			obstacleTimer1 = 0;
			randX = rand() % (350 - (-350) + 1) + (-350);

			App->obstacles->AddObstacle(App->obstacles->bush, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, NOLETHAL_D_OBSTACLE);
		}

		if (obstacleTimer2 < 25) obstacleTimer2++;
		else
		{
			obstacleTimer2 = 0;
			randX = rand() % (350 - (-350) + 1) + (-350);
			randY = rand() % (150 - 80 + 1) + 80;

			if (App->enemies->waveNum < 4) App->obstacles->AddObstacle(App->obstacles->rock, ((float)SCREEN_WIDTH / 2.0f), (float)randX, (float)randY, D_OBSTACLE);
			else App->obstacles->AddObstacle(App->obstacles->tree, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, D_OBSTACLE);
		}
	}
}

void ModuleStage::EndingAndScoreBoard()
{
	SDL_Rect fullScreen = { 0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE };
	App->renderer->DrawQuad(fullScreen, 0, 0, 0, 80);

	int charWidth = App->fontManager->redFont->characterWidth;
	int charHeight = App->fontManager->redFont->characterHeight;
	string end;

	if (App->player->lives > 0)
	{
		end = "YOU WON!";
		App->fontManager->greenFont->printText(end.c_str(), (float)((SCREEN_WIDTH / 2) - (end.length()*charWidth / 2)), 35.0f);
	}
	else
	{
		end = "GAME OVER";
		App->fontManager->redFont->printText(end.c_str(), (float)((SCREEN_WIDTH / 2) - (end.length()*charWidth / 2)), 35.0f);
	}
	
	int counter = 0;
	for (std::map<const char*, int>::iterator it = scores.begin(); it != scores.end(); ++it)
	{
		App->fontManager->redFont->printText(it->first, (float)(SCREEN_WIDTH / 5), scoreBoardPosition + (charHeight + 5)*counter);
		App->fontManager->redFont->printText(to_string(it->second).c_str(), (float)((3 * SCREEN_WIDTH) / 5), (float)(scoreBoardPosition + (charHeight + 5)*counter));
		counter++;
	}

	string player = "YOU";
	App->fontManager->redFont->printText(player.c_str(), (SCREEN_WIDTH / 5), scoreBoardPosition + (charHeight + 5)*counter);
	App->fontManager->redFont->printText(to_string((int)App->player->playerScore).c_str(), ((3 * SCREEN_WIDTH) / 5), scoreBoardPosition + (charHeight + 5)*counter);
		
	if (scoreBoardPosition > 80.0f) scoreBoardPosition -= 1.0f;
	else
	{
		string thanks = "THANKS FOR PLAYING";
		App->fontManager->blueFont->printText(thanks.c_str(), (float)((SCREEN_WIDTH / 2) - (thanks.length()*charWidth / 2)), (float)(4*SCREEN_HEIGHT) / 5);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_intro, this, 4.0f);
	}
}