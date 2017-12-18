#ifndef __MODULESCENEINTRO_H__
#define __MODULESCENEINTRO_H__

#include "Module.h"
#include "Font.h"

struct SDL_Texture;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool active = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	SDL_Texture* menuTexture = nullptr;
	SDL_Rect backgroundRect;
	SDL_Rect screenSize;

	uint fx = 0;
};

#endif // __MODULESCENEINTRO_H__