#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneIntro.h"
#include "FontManager.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModuleSceneIntro::ModuleSceneIntro(bool active) : Module(active)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading space intro");
	
	background = App->textures->Load("assets/intro.png");	
	blueFont = App->fontManager->addReference("blue");

	if(fx == 0)
		fx = App->audio->LoadFx("assets/introSound.wav");

	App->audio->PlayFx(fx);
	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	return true;
}

// UnLoad assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading space scene");

	App->textures->Unload(background);
	
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(background, 0, 0, NULL);
	App->fontManager->printWithFont(blueFont, 150, 150, "SPACE HARRIER RULEZ");

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_space, this, 0.5f);
	}

	return UPDATE_CONTINUE;
}