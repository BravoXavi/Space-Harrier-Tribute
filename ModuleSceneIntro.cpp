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
{
	backgroundRect.x = 1943;
	backgroundRect.y = 57;
	backgroundRect.w = 320;
	backgroundRect.h = 224;

	wavingGuy.frames.push_back({ 17, 5, 51, 35 });
	wavingGuy.frames.push_back({ 69, 5, 51, 35 });
	wavingGuy.frames.push_back({ 17, 40, 51, 35 });
	wavingGuy.frames.push_back({ 69, 5, 51, 35 });
	wavingGuy.speed = 1.5f;

	eye.frames.push_back({ 17, 82, 31, 19 });
	eye.frames.push_back({ 50, 82, 31, 19 });
	eye.frames.push_back({ 84, 82, 31, 19 });
	eye.frames.push_back({ 50, 82, 31, 19 });
	eye.frames.push_back({ 17, 82, 31, 19 });
	eye.frames.push_back({ 17, 103, 31, 19 });
	eye.frames.push_back({ 50, 103, 31, 19 });
	eye.frames.push_back({ 84, 103, 31, 19 });
	eye.frames.push_back({ 50, 103, 31, 19 });
	eye.frames.push_back({ 17, 103, 31, 19 });
	eye.frames.push_back({ 17, 82, 31, 19 });
	eye.frames.push_back({ 17, 123, 31, 19 });
	eye.frames.push_back({ 50, 123, 31, 19 });
	eye.frames.push_back({ 84, 123, 31, 19 });
	eye.frames.push_back({ 50, 123, 31, 19 });
	eye.frames.push_back({ 17, 123, 31, 19 });
	eye.speed = 4.0f;

	title.frames.push_back({ 129, 5, 158, 92 });
	title.frames.push_back({ 129, 102, 158, 92 });
	title.frames.push_back({ 129, 200, 158, 92 });
	title.frames.push_back({ 295, 5, 158, 92 });
	title.frames.push_back({ 295, 102, 158, 92 });
	title.frames.push_back({ 295, 200, 158, 92 });
	title.frames.push_back({ 459, 5, 158, 92 });
	title.frames.push_back({ 459, 102, 158, 92 });
	title.frames.push_back({ 459, 200, 158, 92 });
	title.frames.push_back({ 623, 5, 158, 92 });
	title.frames.push_back({ 623, 102, 158, 92 });
	title.frames.push_back({ 623, 200, 158, 92 });
	title.frames.push_back({ 786, 5, 158, 92 });
	title.frames.push_back({ 786, 102, 158, 92 });
	title.frames.push_back({ 786, 200, 158, 92 });
	title.frames.push_back({ 948, 5, 158, 92 });
	title.frames.push_back({ 948, 102, 158, 92 });
	title.frames.push_back({ 948, 200, 158, 92 });
	title.frames.push_back({ 1110, 5, 158, 92 });
	title.frames.push_back({ 1110, 102, 158, 92 });
	title.frames.push_back({ 1110, 200, 158, 92 });
	title.frames.push_back({ 1275, 5, 158, 92 });
	title.frames.push_back({ 1275, 102, 158, 92 });
	title.frames.push_back({ 1275, 200, 158, 92 });
	title.frames.push_back({ 1439, 5, 158, 92 });
	title.frames.push_back({ 1439, 102, 158, 92 });
	title.frames.push_back({ 1439, 200, 158, 92 });
	title.frames.push_back({ 1603, 5, 158, 92 });
	title.frames.push_back({ 1603, 102, 158, 92 });
	title.frames.push_back({ 1603, 200, 158, 92 });
	title.frames.push_back({ 1766, 5, 158, 92 });
	title.frames.push_back({ 1766, 102, 158, 92 });
	title.speed = 4.0f;
	title.loop = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Menu");
	
	menuTexture = App->textures->Load("assets/MenuFull.png");

	screenSize = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	if(fx == 0)
		fx = App->audio->LoadFx("assets/sfx/SFX_Intro.wav");

	App->audio->StopMusic();
	App->audio->PlayFx(fx);

	title.animationWithoutLoopEnded = false;
	title.Reset();

	return true;
}

// UnLoad assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Menu");
	App->textures->Unload(menuTexture);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(menuTexture, 0.0f, 0.0f, &backgroundRect);
	App->renderer->Blit(menuTexture, 236, 38, &(wavingGuy.GetCurrentFrame()));
	App->renderer->Blit(menuTexture, 67, 103, &(eye.GetCurrentFrame()));
	App->renderer->Blit(menuTexture, 81, 2, &(title.GetCurrentFrame()));

	App->fontManager->blueFont->printText("PRESS ENTER", (SCREEN_WIDTH/2) - 40, SCREEN_HEIGHT/2 + 16);

	if(App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->fade->isFading() == false)
		App->fade->FadeToBlack((Module*)App->scene_stage, this, 0.5f);

	return UPDATE_CONTINUE;
}