#ifndef __MODULECOLLISION_H__
#define __MODULECOLLISION_H__

#include <list>
#include <vector>
#include "Module.h"

enum collisionType {
	PLAYER,
	ENEMY,
	P_LASER,
	E_LASER,
	D_OBSTACLE,
	NOLETHAL_D_OBSTACLE,
	ND_OBSTACLE,
	EXPLOSION,
	MAXIMUM
};

struct Collider
{
	Module* modCallback;
	collisionType colType;
	SDL_Rect rect = { 0, 0, 0, 0 };
	
	int depth = 0;
	bool to_delete = false;
	
	Collider(SDL_Rect rectangle, collisionType colType, int depth, Module* callback) : // expand this call if you need to
		rect(rectangle),
		colType(colType),
		depth(depth),
		modCallback(callback)
	{}

	void SetPos(const int& x, const int& y, const int& z, const int& w, const int& h)
	{
		rect.x = x * SCREEN_SIZE;
		rect.y = y * SCREEN_SIZE;
		depth = z;
		rect.w = w;
		rect.h = h;
	}

	bool CheckCollision(const SDL_Rect& r, const int& z) const;
};

class ModuleCollision : public Module
{
public:
	ModuleCollision();
	~ModuleCollision();

	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	void DebugDraw();
	Collider* AddCollider(const SDL_Rect& rect, collisionType colType, int z, Module* callback);	

private:
	std::list<Collider*> colliders;
	bool debug = false;
	bool collisionMatrix[MAXIMUM][MAXIMUM];
};

#endif // __MODULECOLLISION_H__