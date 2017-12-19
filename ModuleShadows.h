#ifndef __MODULESHADOWS_H__
#define __MODULESHADOWS_H__

#include "Module.h"

struct SDL_Texture;

class ModuleShadows : public Module
{
public:
	ModuleShadows(bool active = true);
	~ModuleShadows();

	bool Start();
	update_status PreUpdate();
	bool CleanUp();

	void DrawShadow(const float& x, const float& y, const float& z, const float& width = -1.0f, const float& height = -1.0f);

private:
	SDL_Texture* graphics = nullptr;
	SDL_Rect shadowPosition;
};

#endif // __MODULESHADOWS_H__