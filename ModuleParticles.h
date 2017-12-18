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

	void Update(const int& updateSelector);
	void setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, SDL_Rect* section, SDL_Rect* resize) const;
	void setResizeRect(const float& w, const float& h) const;

	bool to_delete = false;
	float speed;
	unsigned int fxIndex;
		
	fPoint position = { 0.0f, 0.0f, 0.0f };
	fPoint targetOffset = { 0.0f, 0.0f, 0.0f };
	Animation anim;
	collisionType colType;
	Collider* collider = nullptr;

	BlitTarget* rect = new BlitTarget(nullptr, 0.0f, 0.0f, 0.0f, nullptr, nullptr);
	SDL_Rect* resizeRect = new SDL_Rect({ 0, 0, 0, 0 });
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

	bool onCollision(Collider* c1, Collider* c2);
	void AddParticle(const Particle& particle, float x, float y, float z, collisionType colType);

private:
	std::list<Particle*> active;

public:
	SDL_Texture* graphics = nullptr;
	Particle p_laser;
	Particle e_laser;
	Particle explosion;
};

#endif // __MODULEPARTICLES_H__