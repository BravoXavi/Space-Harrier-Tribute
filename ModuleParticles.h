#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include<list>
#include "Globals.h"
#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "ModuleCollision.h"
#include "ModuleRender.h"

struct SDL_Texture;

struct Particle
{
	Particle();
	Particle(const Particle& p);
	~Particle();

	void setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth);
	void setResizeRect(int x, int y, int w, int h);
	void Update();

	bool to_delete = false;
	unsigned int fxIndex;
	int speed;
	fPoint position = { 0, 0 };
	int z = 0;

	Animation anim;
	collisionType colType;

	// TODO 11: Add an optional collider to each particle -- DONE
	Collider* collider = nullptr;

	BlitTarget* rect = new BlitTarget(nullptr, 0, 0, nullptr, nullptr, 0);
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });
};

class ModuleParticles : public Module
{
public:
	ModuleParticles();
	~ModuleParticles();

	bool Start();
	update_status PreUpdate(); // clear dirty particles
	update_status Update(); // draw
	bool CleanUp();

	void AddParticle(const Particle& particle, int x, int y, collisionType colType, int depth); // feel free to expand this call

private:
	
	std::list<Particle*> active;

public:
	SDL_Texture* graphics = nullptr;
	Particle cannon;
	// prototype particles go here ...
};

#endif // __MODULEPARTICLES_H__