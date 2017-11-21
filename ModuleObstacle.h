#ifndef __MODULEOBSTACLE_H__
#define __MODULEOBSTACLE_H__

#include<list>
#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

struct SDL_Texture;

struct Obstacle
{
	// TODO 1: Fill in the structure with all the properties you need for each particle
	bool to_delete = false;
	iPoint position = { 0, 0 };
	int z = 0;
	int lineToFollow = 0;

	Animation anim;
	collisionType colType;

	// TODO 11: Add an optional collider to each particle -- DONE
	Collider* collision = nullptr;

	Obstacle();
	Obstacle(const Obstacle& p);
	~Obstacle();

	void setResizeRect(int x, int y, int w, int h);
	void setRect(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, int depth);
	void Update();

	int counter = 1;

	BlitTarget* rect = new BlitTarget(nullptr, 0, 0, nullptr, nullptr, 0);	
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });
};

class ModuleObstacle : public Module
{
public:
	ModuleObstacle();
	~ModuleObstacle();

	bool Start();
	update_status PreUpdate(); // clear dirty particles
	update_status Update(); // draw
	bool CleanUp();

	void AddObstacle(const Obstacle& particle, int x, int y); // feel free to expand this call

private:

	std::list<Obstacle*> active;

public:
	SDL_Texture* graphics = nullptr;
	Obstacle tree;
	Obstacle rock;
	Obstacle tower;
	// prototype particles go here ...
};

#endif // __MODULEOBSTACLE_H__