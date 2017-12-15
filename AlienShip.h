#ifndef __ALIENSHIP_H__
#define __ALIENSHIP_H__

#include "Globals.h"
#include "Enemy.h"

class AlienShip : public Enemy
{
public:
	AlienShip();
	AlienShip(const Enemy& alienShip, const fPoint& pos);
	~AlienShip();

	void Update();
	void selectMovementPatron(const int& moveSelector);
	Enemy* createEnemyInstance(const Enemy& e, const fPoint& pos) const;

private:
	float oscillationAngle = 0.0f;
	float oscillationRadius = (float)SCREEN_WIDTH / 2.0f;
	float oscillationSpeed = 0.03f;
};

#endif // __ALIENSHIP_H__