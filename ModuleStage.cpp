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
	scores["ZELLERYON"] = 170000;
	scores["NEIKAR"] = 120000;
	scores["PSYHOLIC"] = 80000;
	scores["DOGJOE"] = 1;
}

ModuleStage::~ModuleStage()
{}

// Load assets
bool ModuleStage::Start()
{
	LOG("Loading Game Stage");

	App->collision->Enable();
	App->player->Enable();
	App->particles->Enable();
	App->enemies->Enable();
	App->obstacles->Enable();
	App->shadows->Enable();
	App->audio->Enable();

	enemySpawnTimer = SDL_GetTicks();
	
	floor = App->textures->Load("assets/Floor.png");
	background = App->textures->Load("assets/backgroundlvl1.png");
	backgroundFront = App->textures->Load("assets/backgroundlvl2.png");
	gui = App->textures->Load("assets/GUI.png");

	introVoiceSFX = App->audio->LoadFx("assets/sfx/VOICE_Welcome.wav");
	App->audio->PlayFx(introVoiceSFX);
	App->audio->PlayMusic("assets/music/MOOT_Theme.ogg", 1.0f);

	Ending = false;
	App->renderer->stopUpdating = false;
	actualScoreBanner.x = 1;
	actualScoreBanner.y = 0;
	actualScoreBanner.w = 48;
	actualScoreBanner.h = 14;
	topScoreBanner = { 9, 15, 32, 14 };
	liveIcon = { 20, 31, 10, 16 };	
	scoreBoardPosition = (float)SCREEN_HEIGHT;

	if (topScore < 10000)
		topScore = 10000;

	return true;
}

// UnLoad assets
bool ModuleStage::CleanUp()
{
	bool ret = true;

	LOG("Unloading Game Stage");

 	App->textures->Unload(floor);
	App->textures->Unload(background);
	App->textures->Unload(backgroundFront);
	App->textures->Unload(gui);
	App->player->Disable();
	App->collision->Disable();
	App->particles->Disable();
	App->enemies->Disable();
	App->obstacles->Disable();

	return ret;
}

//Draw Floor, Background, and extras (All this parts will ALWAYS stay in the back of the screen)
update_status ModuleStage::PreUpdate()
{
	App->renderer->BackgroundBlit(background, 0.2f, 1);
	App->renderer->BackgroundBlit(backgroundFront, 0.5f, 2);
	App->renderer->FloorBlit(floor, nullptr);

	return UPDATE_CONTINUE;
}

// Update of the main stage
update_status ModuleStage::Update()
{
	//Main thread of the Game
	tickUpdate = SDL_GetTicks();
	PrintUI();
	
	//If the player tops the topScore, it'll start incresing with his own one
	if ((int)App->player->playerScore > topScore) 
		topScore = (int)App->player->playerScore;

	//If the player has any lives left, the game will continue spawning enemywaves, obstacles and even a final boss.
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
					if (App->enemies->waveNum == 8)
					{
						App->enemies->bossEncounter = true;
						App->audio->PlayMusic("assets/music/MOOT_BossTheme.ogg", 0.0f);
					}
				}
			}
			else
			{
				if (tickUpdate - enemySpawnTimer > 6000.0f && !App->enemies->aliveEnemy)
				{
					App->enemies->triggerEnemies = true;
					App->enemies->waveNum++;
				}
			}
		}
		else if (App->enemies->bossEncounter && !App->enemies->aliveEnemy)
		{
			//If the player defeated the final boss of the stage, a Victory ending screen will appear with the scoreboard
			EndingAndScoreBoard();
		}
	}
	else
	{
		//Also, if the player does not have any live left, the ending screen will pop out with a Game Over message
		EndingAndScoreBoard();
	}

	//While we keep on playing, obstacles will be generated with a certain random degree
	if(!App->renderer->stopUpdating) 
		GenerateObstacles();

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
	int randX = 0;
	int randY = 0;

	if (!bossActive)
	{
		if (obstacleTimer1 < 15) 
			obstacleTimer1++;
		else
		{
			obstacleTimer1 = 0;
			randX = rand() % (350 - (-350) + 1) + (-350);

			App->obstacles->AddObstacle(App->obstacles->bush, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, NOLETHAL_D_OBSTACLE);
		}

		if (obstacleTimer2 < 25)
			obstacleTimer2++;
		else
		{
			obstacleTimer2 = 0;
			randX = rand() % (350 - (-350) + 1) + (-350);
			randY = rand() % (150 - 80 + 1) + 80;

			if (App->enemies->waveNum < 4) 
				App->obstacles->AddObstacle(App->obstacles->rock, ((float)SCREEN_WIDTH / 2.0f), (float)randX, (float)randY, D_OBSTACLE);
			else 
				App->obstacles->AddObstacle(App->obstacles->tree, ((float)SCREEN_WIDTH / 2.0f), (float)randX, 0.0f, D_OBSTACLE);
		}
	}
}

void ModuleStage::EndingAndScoreBoard()
{
	if (!Ending)
	{
		App->audio->StopMusic();
		App->renderer->stopUpdating = true;
		Ending = true;
	}

	string end;
	int charWidth = App->fontManager->redFont->characterWidth;
	int charHeight = App->fontManager->redFont->characterHeight;
	SDL_Rect fullScreen = { 0, 0, SCREEN_WIDTH*SCREEN_SIZE, SCREEN_HEIGHT*SCREEN_SIZE };
	
	App->renderer->DrawQuad(fullScreen, 0, 0, 0, 80);

	if (App->player->lives > 0)
	{
		if( !App->audio->isMusicPlaying()) 
			App->audio->PlayMusic("assets/music/MUSIC_Victory.ogg", 0.0f);

		end = "YOU WON!";
		App->fontManager->greenFont->printText(end.c_str(), (float)((SCREEN_WIDTH / 2) - (end.length()*charWidth / 2)), 35.0f);
	}
	else
	{
		if (!App->audio->isMusicPlaying()) 
			App->audio->PlayMusic("assets/music/MUSIC_GameOver.ogg", 0.0f);

		end = "GAME OVER";
		App->fontManager->redFont->printText(end.c_str(), (float)((SCREEN_WIDTH / 2) - (end.length()*charWidth / 2)), 35.0f);
	}
	
	int counter = 0;
	bool playerAdded = false;
	string player = "YOU";

	for (std::map<const char*, int>::iterator it = scores.begin(); it != scores.end(); ++it)
	{
		if (App->player->playerScore > it->second && !playerAdded)
		{
			App->fontManager->redFont->printText(player.c_str(), (SCREEN_WIDTH / 5), scoreBoardPosition + (charHeight + 5)*counter);
			App->fontManager->redFont->printText(to_string((int)App->player->playerScore).c_str(), ((3 * SCREEN_WIDTH) / 5), scoreBoardPosition + (charHeight + 5)*counter);
			playerAdded = true;
			counter++;
		}
		App->fontManager->redFont->printText(it->first, (float)(SCREEN_WIDTH / 5), scoreBoardPosition + (charHeight + 5)*counter);
		App->fontManager->redFont->printText(to_string(it->second).c_str(), (float)((3 * SCREEN_WIDTH) / 5), (float)(scoreBoardPosition + (charHeight + 5)*counter));
		counter++;
	}
		
	if (scoreBoardPosition > 80.0f) 
		scoreBoardPosition -= 1.0f;
	else
	{
		string thanks = "THANKS FOR PLAYING";
		App->fontManager->blueFont->printText(thanks.c_str(), (float)((SCREEN_WIDTH / 2) - (thanks.length()*charWidth / 2)), (float)(4*SCREEN_HEIGHT) / 5);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false)
		App->fade->FadeToBlack((Module*)App->scene_intro, this, 3.0f);
}