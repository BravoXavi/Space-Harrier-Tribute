#ifndef __MODULEENEMY_H__
#define __MODULEENEMY_H__

#include<list>
#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

struct SDL_Texture;

struct Enemy
{
	// TODO 1: Fill in the structure with all the properties you need for each particle
	bool to_delete = false;
	unsigned int fxIndex;
	int speed;
	iPoint position = { 0, 0 };
	int z = 0;

	Animation anim;
	collisionType colType;

	// TODO 11: Add an optional collider to each particle -- DONE
	Collider* collision = nullptr;

	Enemy();
	Enemy(const Enemy& p);
	~Enemy();

	void Update();

	BlitTarget* rect = new BlitTarget(nullptr, 0, 0, nullptr, nullptr, 0);

	void setRect(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, int depth);
};

class ModuleEnemy : public Module
{
public:
	ModuleEnemy();
	~ModuleEnemy();

	bool Start();
	update_status PreUpdate(); // clear dirty particles
	update_status Update(); // draw
	bool CleanUp();

	void AddEnemy(const Enemy& enemy, int x, int y); // feel free to expand this call

private:

	SDL_Texture* graphics = nullptr;
	std::list<Enemy*> active;

public:
	Enemy alienShip;
};

#endif // __MODULEENEMY_H__