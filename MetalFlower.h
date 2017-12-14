#ifndef __METALFLOWER_H__
#define __METALFLOWER_H__

#include "Globals.h"
#include "Enemy.h"

class MetalFlower : public Enemy
{
public:
	MetalFlower();
	MetalFlower(const Enemy& alienShip);
	~MetalFlower();

	void Update();
	void selectMovementPatron(const int& moveSelector);
	Enemy* createEnemyInstance(const Enemy& e) const;
};

#endif // __METALFLOWER_H__