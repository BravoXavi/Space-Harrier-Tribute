#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

struct SDL_Texture;

class Enemy 
{
public:

	Enemy() {}
	Enemy(const Enemy& p) {}
	virtual ~Enemy() {}

	virtual void Update() = 0;
	virtual Enemy* createEnemyInstance(const Enemy& e, const fPoint& pos) const = 0;
	virtual void selectMovementPatron(const int& moveSelector) = 0;
	virtual void setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const
	{
		rect->x = x;
		rect->y = y;
		rect->z = z;
		rect->newWidth = newWidth;
		rect->newHeight = newHeight;
		rect->texture = texture;
		rect->section = section;
	}

public:
	BlitTarget* rect = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr);
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });
	collisionType colType;
	Collider* collider = nullptr;
	fPoint worldPosition = { 0.0f, 0.0f, 0.0f };
	fPoint screenPosition = { 0.0f, 0.0f, 0.0f };

	Animation enemyAnimation;

	Uint32 animationTimer = 0;
	float uniDimensionalSpeed = 0.0f;
	float depthSpeed = 0.0f;
	int moveSet = 0;
	bool to_delete = false;
	unsigned int fxIndex;
	bool invulnerable = false;

};

#endif // __ENEMY_H__