#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleFadeToBlack.h"
#include "ModuleCollision.h"
#include "ModuleParticles.h"
#include "ModuleTime.h"
#include "ModuleSegaIntro.h"
#include "ModuleSceneIntro.h"
#include "ModuleSceneSpace.h"
#include "ModulePlayer.h"
#include "ModuleEnemy.h"
#include "ModuleObstacle.h"
#include "ModuleShadows.h"
#include "FontManager.h"
#include "Font.h"

using namespace std;

Application::Application()
{
	// Order matters: they will init/start/pre/update/post in this order
	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(audio = new ModuleAudio());
	modules.push_back(time = new ModuleTime());
	fontManager = new FontManager();

	// Game Modules
	modules.push_back(sega_intro = new ModuleSegaIntro(false));
	modules.push_back(scene_intro = new ModuleSceneIntro(false));
	modules.push_back(scene_space = new ModuleSceneSpace(false));
	modules.push_back(obstacles = new ModuleObstacle());
	modules.push_back(shadows = new ModuleShadows());
	modules.push_back(enemies = new ModuleEnemy());
	modules.push_back(player = new ModulePlayer(false));
	
	// Modules to draw on top of game logic
	modules.push_back(collision = new ModuleCollision());
	modules.push_back(particles = new ModuleParticles());
	modules.push_back(fade = new ModuleFadeToBlack());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		RELEASE(*it);

	RELEASE(fontManager);
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init(); // we init everything, even if not anabled

	fontManager->Init();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if((*it)->IsEnabled() == true)
			ret = (*it)->Start();
	}

	// Start the first scene --
	fade->FadeToBlack(scene_space, nullptr, 0.0f);

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		if (App->paused == false)
		{
			App->renderer->DrawPauseScreen();
			App->paused = true;
		}
		else if (App->paused == true)
		{
			App->paused = false;
		}
	}

	if (!paused)
	{
		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			if ((*it)->IsEnabled() == true)
				ret = (*it)->PreUpdate();

		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			if ((*it)->IsEnabled() == true)
				ret = (*it)->Update();

		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			if ((*it)->IsEnabled() == true)
				ret = (*it)->PostUpdate();
	}
	else
	{
		ret = input->PreUpdate();
		if (ret == UPDATE_CONTINUE)
		{
			ret = input->Update();
			ret = input->PostUpdate();
			ret = renderer->PostUpdate();
		}
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		if((*it)->IsEnabled() == true) 
			ret = (*it)->CleanUp();

	fontManager->CleanUp();

	return ret;
}

