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

private:
	const void checkHorizontalAnimation(bool running = false);
	const bool onCollision(Collider* c1, Collider* c2);
	const void modifyHorizonY() const;
	const void setCharSpeed();
	const void moveCollider() const;
	const void LoseOneLive();

public:
	int lives = 0;
	bool gotHit = false;
	bool gotTrip = false;
	float playerScore = 0.0f;

	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	Collider* collider = nullptr;
	fPoint position = { 0.0f, 0.0f, 0.0f };	
	
private:
	int playerWidth = 0;
	int playerHeight = 0;
	bool invulnerableState = false;
	bool destroyed = false;
	bool godMode = false;
	const static float playerDepth;

	Animation run;
	Animation middle;
	Animation left1;
	Animation left2;
	Animation right1;
	Animation right2;
	Animation tripping;
	Animation hit;
	uint deathSFX;
	uint getreadySFX;
	uint tripSFX;
	Uint32 initAnimationTimer = 0;
	Uint32 invulnerableTimer = 0;

};

#endif