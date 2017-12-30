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
#include "DragonPart.h"

struct SDL_Texture;

class ModuleEnemy : public Module
{
public:
	ModuleEnemy(bool active = false);
	~ModuleEnemy();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	
	void AddEnemy(const Enemy& enemy, const float& x, const float& y, const float& z, collisionType colType, const int& moveSet, const float& oscillationAngle = 0.0f);
	void AddModularEnemy(const Enemy& head, const Enemy& body, const Enemy& tail, float x, float y, float z, const int& moveSet, const int& bodySize, const float& oscillationAngle = 0.0f);
	const void enemyWave(const int& selector);

private:
	const bool onCollision(Collider* c1, Collider* c2);

public:
	int waveNum = 0;
	bool triggerEnemies = false;
	bool bossEncounter = false;
	bool aliveEnemy = false;
	SDL_Texture * graphics = nullptr;
	
private:
	int enemyWaveCount = 0;
	uint spaceshipSFX;
	uint tomosSFX;
	std::list<Enemy*> active;
	Uint32 spawnTimer;
	AlienShip alienShip;
	MetalFlower tomos;
	DragonPart dragonHead;
	DragonPart dragonBody;
	DragonPart dragonTail;

};

#endif // __MODULEENEMY_H__