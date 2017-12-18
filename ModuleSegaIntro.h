#ifndef __MODULESEGAINTRO_H__
#define __MODULESEGAINTRO_H__

#include "Module.h"
#include "Font.h"

struct SDL_Texture;

class ModuleSegaIntro : public Module
{
public:
	ModuleSegaIntro(bool active = true);
	~ModuleSegaIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	SDL_Texture* logo = nullptr;
	Uint32 segaTimer = 0;
	uint fx = 0;
};

#endif // __MODULESEGAINTRO_H__