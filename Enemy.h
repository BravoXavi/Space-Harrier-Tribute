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
	virtual Enemy* createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& colType, const int& moveSelector, const float& oscillationAngle) const = 0;
	virtual void setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const
	{
		dataToBlit->x = x;
		dataToBlit->y = y;
		dataToBlit->z = z;
		dataToBlit->newWidth = newWidth;
		dataToBlit->newHeight = newHeight;
		dataToBlit->texture = texture;
		dataToBlit->section = section;
	}
	
private:
	virtual const void selectMovementPatron(const int& moveSelector) = 0;

public:
	int moveSet = 1;
	int lifePoints = 1;
	bool to_delete = false;
	float depthSpeed = 0.0f;
	float oscillationAngle = 0.0f;
	float oscillationRadius = 0.0f;
	float oscillationSpeed = 0.0f;
	float uniDimensionalSpeed = 0.0f;

	fPoint screenPosition = { 0.0f, 0.0f, 0.0f };
	fPoint worldPosition = { 0.0f, 0.0f, 0.0f };
	Collider* collider = nullptr;
	collisionType colType;
	Animation enemyAnimation;
	Uint32 animationTimer = 0;
	Enemy * superiorBodyPart = nullptr;
	BlitTarget* dataToBlit = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr);

};

#endif // __ENEMY_H__