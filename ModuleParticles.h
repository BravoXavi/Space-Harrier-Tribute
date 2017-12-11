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

	void p_laser_Update();
	void e_laser_Update();
	void explosion_Update();

	void setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth);
	void setResizeRect(int x, int y, int w, int h);

	bool to_delete = false;
	unsigned int fxIndex;
	float speed;	

	fPoint position = { 0, 0, 0 };
	fPoint targetOffset = { 0, 0, 0 };
	Animation anim;
	collisionType colType;
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

	bool onCollision(Collider* c1, Collider* c2);
	void AddParticle(const Particle& particle, int x, int y, collisionType colType, int depth); // feel free to expand this call

private:
	
	std::list<Particle*> active;

public:
	SDL_Texture* graphics = nullptr;
	Particle p_laser;
	Particle e_laser;
	Particle explosion;
	// prototype particles go here ...
};

#endif // __MODULEPARTICLES_H__