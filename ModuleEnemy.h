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
	void setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, SDL_Rect* section, SDL_Rect* resize) const;
	void setResizeRect(const float& w, const float& h) const;

	int speed;
	int moveSet = 0;
	bool to_delete = false;
	unsigned int fxIndex;
	
	//DEBUG
	int attackCharged = 0;

	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;
	fPoint position = { 0.0f, 0.0f, 0.0f };
	BlitTarget* rect = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, nullptr, nullptr);
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });	
};

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

private:
	std::list<Enemy*> active;

public:
	SDL_Texture* graphics = nullptr;
	Enemy alienShip;
	Enemy metalFlower;
	Enemy dragonBoss;
};

#endif // __MODULEENEMY_H__