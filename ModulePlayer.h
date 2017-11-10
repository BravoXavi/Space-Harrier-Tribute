#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"

struct SDL_Texture;

class ModulePlayer : public Module
{
public:
	ModulePlayer(bool active = true);
	~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;

	Animation run;
	Animation middle;
	Animation left1;
	Animation left2;
	Animation right1;
	Animation right2;

	Animation* activeCharAnimation = nullptr;

	iPoint position;
	bool destroyed = false;

private:
	void checkHorizontalAnimation();
	void modifyHorizonY();
	void setCharSpeed(float speed);
};

#endif