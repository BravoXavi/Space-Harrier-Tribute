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

	void Update();

	float calculateScaleValue(float yRender);
	void setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const;

	int lineToFollow = 0;
	bool to_delete = false;
	bool shadowCast = false;
	float xOffset = 0.0f;	

	fPoint screenPosition = { 0.0f, 0.0f, 0.0f };
	fPoint worldPosition = { 0.0f, 0.0f, 0.0f };
	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;
	BlitTarget* dataToBlit = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr);	
};

class ModuleObstacle : public Module
{
public:
	ModuleObstacle();
	~ModuleObstacle();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	bool onCollision(Collider* c1, Collider* c2);
	void AddObstacle(const Obstacle& particle, float x, float xOffset, float y, collisionType type);

private:
	std::list<Obstacle*> active;

public:
	SDL_Texture* graphics = nullptr;
	SDL_Texture* models = nullptr;
	Obstacle tree;
	Obstacle rock;
	Obstacle bush;
};

#endif // __MODULEOBSTACLE_H__