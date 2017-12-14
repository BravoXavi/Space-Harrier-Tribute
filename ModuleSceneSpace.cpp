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

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModuleSceneSpace::ModuleSceneSpace(bool active) : Module(active)
{}

ModuleSceneSpace::~ModuleSceneSpace()
{}

// Load assets
bool ModuleSceneSpace::Start()
{
	LOG("Loading space scene");
	
	floor = App->textures->Load("assets/Floor.png");
	background = App->textures->Load("assets/Background3Scaled.png");
	backgroundFront = App->textures->Load("assets/backgroundFront.png");
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
		
	//if (timeCounter < 20) timeCounter++;
	//else 
	//{
	//	timeCounter = 0;
	//	int randNumX2 = rand() % (300 - (-300) + 1) + (-300);

	//	App->obstacles->AddObstacle(App->obstacles->bush, ((float)SCREEN_WIDTH / 2.0f), (float)randNumX2, 0.0f, NOLETHAL_D_OBSTACLE);
	//	//App->obstacles->AddObstacle(App->obstacles->tree, ((float)SCREEN_WIDTH / 2.0f), (float)randNumX2, 0.0f, D_OBSTACLE);
	//}

	//if (timeCounter2 < 50) timeCounter2++;
	//else
	//{
	//	timeCounter2 = 0;
	//	int randNumX = rand() % (200 - (-200) + 1) + (-200);
	//	int randNumY = rand() % (170 - 80 + 1) + 80;

	//	App->obstacles->AddObstacle(App->obstacles->rock, ((float)SCREEN_WIDTH / 2.0f), (float)randNumX, (float)randNumY, WALL);
	//}

		//App->enemies->AddEnemy(App->enemies->alienShip, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, 12.0f, ENEMY, 1);

	//if (!App->enemies->aliveWave)
	//{
	//	App->enemies->enemyWave(App->enemies->waveNum);
	//	App->enemies->aliveWave = true;
	//}

	//------------------------------------------------------------------------------

	Uint32 tickUpdate = SDL_GetTicks();

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
		if (tickUpdate - enemySpawnTimer > 6000.0f)
		{
			App->enemies->triggerEnemies = true;
			App->enemies->waveNum++;
			LOG("NEXT WAVE -> Wave %i -------------------", App->enemies->waveNum);
		}
	}

	PrintUI();

	return UPDATE_CONTINUE;
}

void ModuleSceneSpace::PrintUI() const
{
	App->fontManager->blueFont->printText("STAGE 1", SCREEN_WIDTH - 57, SCREEN_HEIGHT - 9, 0.8f);
}