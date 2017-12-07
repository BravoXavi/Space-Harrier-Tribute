#ifndef __MODULESCENESPACE_H__
#define __MODULESCENESPACE_H__

#include "Module.h"
#include <list>

struct SDL_Texture;

class ModuleSceneSpace : public Module
{
public:
	ModuleSceneSpace(bool active = true);
	~ModuleSceneSpace();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

public:
	uint startFx;
	SDL_Texture* floor = nullptr;
	SDL_Texture* background = nullptr;
	SDL_Texture* backgroundFront = nullptr;

	int timeCounter = 0;
	int timeCounter2 = 0;
};

#endif // __MODULESCENESPACE_H__