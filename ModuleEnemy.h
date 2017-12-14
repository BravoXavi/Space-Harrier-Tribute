#ifndef __MODULEENEMY_H__
#define __MODULEENEMY_H__

#include <list>
#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"
#include "Enemy.h"
#include "AlienShip.h"
#include "MetalFlower.h"

struct SDL_Texture;

class ModuleEnemy : public Module
{
public:
	ModuleEnemy();
	~ModuleEnemy();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	bool onCollision(Collider* c1, Collider* c2);
	void AddEnemy(const Enemy& enemy, float x, float y, float z, collisionType colType, int moveSet);
	void enemyWave(const int& selector);

private:
	std::list<Enemy*> active;
	Uint32 spawnTimer = 0;
	int enemyWaveCount = 0;

public:
	SDL_Texture* graphics = nullptr;

	AlienShip alienShip;
	MetalFlower tomos;

	int waveNum = 0;
	bool triggerEnemies = false;
	bool bossEncounter = false;
	
};

#endif // __MODULEENEMY_H__