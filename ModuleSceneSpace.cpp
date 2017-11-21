#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCollision.h"
#include "ModuleParticles.h"
#include "ModuleSceneSpace.h"
#include "ModuleObstacle.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModuleSceneSpace::ModuleSceneSpace(bool active) : Module(active)
{}

ModuleSceneSpace::~ModuleSceneSpace()
{}

// Load assets
bool ModuleSceneSpace::Start()
{
	LOG("Loading space scene");
	
	floor = App->textures->Load("assets/Floor.bmp");
	startFx = App->audio->LoadFx("assets/initVoice.wav");

	App->player->Enable();
	App->particles->Enable();
	App->collision->Enable();

	//App->audio->PlayFx(startFx);
	//App->audio->PlayMusic("assets/Theme.wav", 1.0f);

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
	App->renderer->FloorBlit(floor, 0, 0, nullptr);
	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleSceneSpace::Update()
{
	// Draw everything --------------------------------------
	//App->renderer->Blit(stars, 0, 0, nullptr, nullptr);

	if (timeCounter < 1)
	{
		int randNum = rand() % ((SCREEN_WIDTH - 20) - 20 + 1) + 20;
		App->obstacles->AddObstacle(App->obstacles->tree, randNum, (SCREEN_HEIGHT - App->renderer->horizonY) - App->obstacles->tree.anim.GetCurrentFrame().h);
		timeCounter++;
	}
	else
	{
		timeCounter = 2;

		//Add Obstacle.
		

		
	}
		
	return UPDATE_CONTINUE;
}