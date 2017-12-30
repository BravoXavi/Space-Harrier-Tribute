#ifndef __METALFLOWER_H__
#define __METALFLOWER_H__

#include "Globals.h"
#include "Enemy.h"

class MetalFlower : public Enemy
{
public:
	MetalFlower();
	MetalFlower(const Enemy& alienShip, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oscillationAngle);
	~MetalFlower();

	void Update();
	Enemy* createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oscillationAngle) const;

private:
	const void selectMovementPatron(const int& moveSelector);

};

#endif // __METALFLOWER_H__