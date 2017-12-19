#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModuleFadeToBlack;
class ModuleCollision;
class ModuleParticles;
class ModuleTime;
class FontManager;

// Game modules ---
class ModuleEnemy;
class ModulePlayer;
class ModuleSegaIntro;
class ModuleSceneIntro;
class ModuleSceneSpace;
class ModuleShadows;
class ModuleObstacle;


class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleFadeToBlack* fade;
	ModuleCollision* collision;
	ModuleParticles* particles;
	ModuleTime* time;
	FontManager* fontManager;

	// Game modules ---
	ModuleObstacle* obstacles;
	ModuleShadows* shadows;
	ModuleEnemy* enemies;
	ModulePlayer* player;
	ModuleSegaIntro* sega_intro;
	ModuleSceneIntro* scene_intro;
	ModuleSceneSpace* scene_space;

	bool paused = false;

private:

	std::list<Module*> modules;
};

extern Application* App;

#endif // __APPLICATION_CPP__