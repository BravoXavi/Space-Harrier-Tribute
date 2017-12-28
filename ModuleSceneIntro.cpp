#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneIntro.h"
#include "FontManager.h"

ModuleSceneIntro::ModuleSceneIntro(bool active) : Module(active)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading space intro");
	
	menuTexture = App->textures->Load("assets/good.png");
	backgroundRect.x = 10;
	backgroundRect.y = 601;
	backgroundRect.w = 400;
	backgroundRect.h = 253;

	screenSize = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	if(fx == 0)
		fx = App->audio->LoadFx("assets/sfx/SFX_Intro.wav");

	App->audio->StopMusic();
	App->audio->PlayFx(fx);

	return true;
}

// UnLoad assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro Scene");

	App->textures->Unload(menuTexture);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(menuTexture, 0.0f, 0.0f, nullptr);//&backgroundRect, &screenSize);
	App->fontManager->blueFont->printText("PRESS START", (SCREEN_WIDTH/2) - 40, SCREEN_HEIGHT/2 + 16);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_stage, this, 0.5f);
	}

	return UPDATE_CONTINUE;
}