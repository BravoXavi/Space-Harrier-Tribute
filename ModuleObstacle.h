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
	Obstacle();
	Obstacle(const Obstacle& p);
	~Obstacle();

	void setResizeRect(int x, int y, int w, int h);
	void setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth);
	void Update();

	float calculateScaleValue(float yRender);

	bool to_delete = false;

	fPoint position = { 0.0f, 0.0f };
	float xOffset = 0.0f;
	int z = 0;
	int lineToFollow = 0;

	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;
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

	void AddObstacle(const Obstacle& particle, float x, float xOffset, float y, collisionType type); // feel free to expand this call

private:

	std::list<Obstacle*> active;

public:
	SDL_Texture* graphics = nullptr;
	SDL_Texture* models = nullptr;
	Obstacle tree;
	Obstacle rock;
	Obstacle bush;
	// prototype particles go here ...
};

#endif // __MODULEOBSTACLE_H__