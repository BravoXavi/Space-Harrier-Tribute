#ifndef __MODULESCENESPACE_H__
#define __MODULESCENESPACE_H__

#include "Module.h"
#include "Font.h"
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
	void PrintUI() const;

public:
	uint startFx;
	Uint32 enemySpawnTimer = 0;

	int timeCounter = 0;
	int timeCounter2 = 0;

private:
	SDL_Texture* floor = nullptr;
	SDL_Texture* background = nullptr;
	SDL_Texture* backgroundFront = nullptr;
};

#endif // __MODULESCENESPACE_H__