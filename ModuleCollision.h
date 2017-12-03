#ifndef __ModuleCollision_H__
#define __ModuleCollision_H__

#include <list>
#include <vector>
#include "Module.h"

enum collisionType {
	PLAYER,
	P_LASER,
	E_LASER,
	D_OBSTACLE,
	ND_OBSTACLE,
	MAXIMUM
};

struct Collider
{
	SDL_Rect rect = { 0,0,0,0 };
	collisionType colType;
	int depth = 0;
	bool to_delete = false;
	Module* modCallback;

	// TODO 10: Add a way to notify other classes that a collision happened

	Collider(SDL_Rect rectangle, collisionType colType, int zDepth, Module* callback) : // expand this call if you need to
		rect(rectangle),
		colType(colType),
		depth(zDepth),
		modCallback(callback)
	{}

	void SetPos(int x, int y, int z)
	{
		rect.x = x*SCREEN_SIZE;
		rect.y = y*SCREEN_SIZE;
		depth = z;
	}

	void SetSize(int w, int h)
	{
		rect.w = w*SCREEN_SIZE;
		rect.h = h*SCREEN_SIZE;
	}

	bool CheckCollision(const SDL_Rect& r, const int& depth) const;
};

class ModuleCollision : public Module
{
public:

	ModuleCollision();
	~ModuleCollision();

	update_status PreUpdate();
	update_status Update();

	bool CleanUp();

	Collider* AddCollider(const SDL_Rect& rect, collisionType colType, int zDepth, Module* callback);
	void DebugDraw();

private:

	std::list<Collider*> colliders;
	bool debug = false;
	bool collisionMatrix[MAXIMUM][MAXIMUM];
};

#endif // __ModuleCollision_H__