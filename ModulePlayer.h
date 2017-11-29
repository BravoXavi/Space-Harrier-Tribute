#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"

struct SDL_Texture;
struct Collider;

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

	Collider* collider = nullptr;

	fPoint position;
	bool destroyed = false;

private:
	const static int playerDepth = -1;
	void checkHorizontalAnimation(bool running = false);
	void modifyHorizonY();
	void setCharSpeed();
};

#endif