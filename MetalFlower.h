#ifndef __METALFLOWER_H__
#define __METALFLOWER_H__

#include "Globals.h"
#include "Enemy.h"

class MetalFlower : public Enemy
{
public:
	MetalFlower();
	MetalFlower(const Enemy& alienShip, const fPoint& pos);
	~MetalFlower();

	void Update();
	void selectMovementPatron(const int& moveSelector);
	Enemy* createEnemyInstance(const Enemy& e, const fPoint& pos) const;

private:
	float rotationAngle = 0.0f;
	float spinSpeed = 0.015f;
	float spinRadius = 40.0f;
};

#endif // __METALFLOWER_H__