#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include <list>
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

	void Update(const int& updateSelector);
	const void setDataToBlit(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const;

	bool to_delete = false;
	bool repelled = false;
	float speed = 0;
	unsigned int fxIndex = 0;
		
	fPoint position = { 0.0f, 0.0f, 0.0f };
	fPoint targetOffset = { 0.0f, 0.0f, 0.0f };
	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;
	BlitTarget* dataToBlit = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr);
};

class ModuleParticles : public Module
{
public:
	ModuleParticles();
	~ModuleParticles();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	void AddParticle(const Particle& particle, const float& x, const float& y, const float& z, collisionType colType);

private:
	const bool onCollision(Collider* c1, Collider* c2);
	
public:
	SDL_Texture * graphics = nullptr;
	Particle p_laser;
	Particle e_laser;
	Particle explosion;
	Particle fireBall;

private:
	uint repelledShotSFX = 0;
	std::list<Particle*> active;

};

#endif // __MODULEPARTICLES_H__