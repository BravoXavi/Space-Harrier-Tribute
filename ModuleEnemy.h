#ifndef __MODULEENEMY_H__
#define __MODULEENEMY_H__

#include <list>
#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

struct SDL_Texture;

struct Enemy
{
	Enemy();
	Enemy(const Enemy& p);
	~Enemy();

	void Update();

	bool to_delete = false;
	unsigned int fxIndex;
	int speed;
	int attackCharged = 0;

	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;
	fPoint position = { 0, 0, 0 };
	
	BlitTarget* rect = new BlitTarget(nullptr, 0, 0, nullptr, nullptr, 0);
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });

	void setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth);
	void setResizeRect(int x, int y, int w, int h);
};

class ModuleEnemy : public Module
{
public:
	ModuleEnemy();
	~ModuleEnemy();

	bool Start();
	update_status PreUpdate(); // clear dirty particles
	update_status Update(); // draw
	bool CleanUp();

	bool onCollision(Collider* c1, Collider* c2);
	void AddEnemy(const Enemy& enemy, float x, float y, collisionType colType, float depth);

private:
	std::list<Enemy*> active;

public:
	SDL_Texture* graphics = nullptr;
	Enemy alienShip;
	Enemy metalFlower;
	Enemy dragonBoss;
};

#endif // __MODULEENEMY_H__