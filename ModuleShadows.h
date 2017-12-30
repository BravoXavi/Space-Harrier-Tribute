#ifndef __MODULESHADOWS_H__
#define __MODULESHADOWS_H__

#include "Module.h"

struct SDL_Texture;

class ModuleShadows : public Module
{
public:
	ModuleShadows(bool active = false);
	~ModuleShadows();

	bool Start();
	bool CleanUp();

	const void DrawShadow(const float& x, const float& y, const float& z, const float& playerWidth);

private:
	SDL_Texture* graphics = nullptr;
	SDL_Rect shadowPosition;
};

#endif // __MODULESHADOWS_H__