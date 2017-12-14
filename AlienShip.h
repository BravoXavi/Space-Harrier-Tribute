#ifndef __ALIENSHIP_H__
#define __ALIENSHIP_H__

#include "Globals.h"
#include "Enemy.h"

class AlienShip : public Enemy
{
public:
	AlienShip();
	AlienShip(const Enemy& alienShip);
	~AlienShip();

	void Update();
	void selectMovementPatron(const int& moveSelector);
	Enemy* createEnemyInstance(const Enemy& e) const;
};

#endif // __ALIENSHIP_H__