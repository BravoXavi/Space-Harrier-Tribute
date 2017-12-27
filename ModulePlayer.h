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
	Animation tripping;
	Animation hit;

	Collider* collider = nullptr;
	fPoint position = { 0.0f, 0.0f, 0.0f };	
	
	int lives = 1;
	float playerScore = 0.0f;

	bool gotHit = false;
	bool gotTrip = false;

private:
	Uint32 initAnimationTimer = 0;
	Uint32 invulnerableTimer = 0;
	bool invulnerableState = false;
	void LoseOneLive();

	const static float playerDepth;

	int playerWidth, playerHeight = 0;
	
	bool destroyed;
	void checkHorizontalAnimation(bool running = false);
	bool onCollision(Collider* c1, Collider* c2);
	void modifyHorizonY() const;
	void setCharSpeed();
	void moveCollider() const;
};

#endif