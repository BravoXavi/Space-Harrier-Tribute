#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSegaIntro.h"

ModuleSegaIntro::ModuleSegaIntro(bool active) : Module(active)
{}

ModuleSegaIntro::~ModuleSegaIntro()
{}

bool ModuleSegaIntro::Start()
{
	segaTimer = SDL_GetTicks();

	logo = App->textures->Load("assets/segalogo.png");

	if (fx == 0)
		fx = App->audio->LoadFx("assets/sfx/sega.wav");

	App->audio->PlayFx(fx);

	return true;
}

bool ModuleSegaIntro::CleanUp()
{
	App->textures->Unload(logo);
	return true;
}

update_status ModuleSegaIntro::Update()
{
	App->renderer->Blit(logo, 0.0f, 0.0f, nullptr);

	Uint32 tick = SDL_GetTicks();
	if (tick - segaTimer >= 4000.0f)
	{
		if(App->fade->isFading() == false) App->fade->FadeToBlack((Module*)App->scene_intro, this, 0.5f);
	}
			
	return UPDATE_CONTINUE;
}