#include <math.h>
#include "ModuleParticles.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"

#include "SDL/include/SDL_timer.h"

ModuleParticles::ModuleParticles()
{}

ModuleParticles::~ModuleParticles()
{
	//delete resizeParticleRect;
	//delete particleRect;
}

// Load assets
bool ModuleParticles::Start()
{
	LOG("Loading particles");
	graphics = App->textures->Load("assets/Shoots.png");


	// TODO 2: Create a prototype for the laser particle -- DONE
	// audio: rtype/laser.wav
	// coords: {232, 103, 16, 12}; {249, 103, 16, 12};
	cannon.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	cannon.anim.frames.push_back({ 1, 1, 91, 61 });
	cannon.anim.frames.push_back({ 95, 0, 91, 61 });
	cannon.anim.frames.push_back({ 188, 1, 91, 61 });
	cannon.anim.frames.push_back({ 284, 0, 91, 61 });
	cannon.anim.speed = 0.1f;
	cannon.z = 1;
	cannon.speed = 1;
	cannon.colType = CANNON;
    //cannon.collision = new Collider({ cannon.position.x, cannon.position.y, 16, 12 });

	// TODO 12: Create a new "Explosion" particle -- DONE
	// audio: rtype/explosion.wav
	// coords: {274, 296, 33, 30}; {313, 296, 33, 30}; {346, 296, 33, 30}; {382, 296, 33, 30}; {419, 296, 33, 30}; {457, 296, 33, 30};

	return true;
}

// Unload assets
bool ModuleParticles::CleanUp()
{
	LOG("Unloading particles");
	App->textures->Unload(graphics);
	
	for (list<Particle*>::iterator it = active.begin(); it != active.end(); ++it)
		RELEASE(*it);

	active.clear();
	
	return true;
}

// PreUpdate to clear up all dirty particles
update_status ModuleParticles::PreUpdate()
{
	for (list<Particle*>::iterator it = active.begin(); it != active.end();)
	{
		if ((*it)->to_delete == true)
		{
			RELEASE(*it);
			it = active.erase(it);
		}
		else
			++it;
	}

	return UPDATE_CONTINUE;
}

// Update all particle logic and draw them
update_status ModuleParticles::Update()
{
	for (list<Particle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		Particle* p = *it;

		p->Update();

		float zModifier = 1.0f - ((float)p->z / MAX_Z);
		int newWidth = (int)(p->anim.GetCurrentFrame().w * zModifier);
		int newHeight = (int)(p->anim.GetCurrentFrame().h * zModifier);
		int xMove = (p->anim.GetCurrentFrame().w - newWidth) / 2;
		int yMove = (p->anim.GetCurrentFrame().h - newHeight) / 2;

		p->setResizeRect(0, 0, newWidth, newHeight);
		p->setRect(graphics, p->position.x + xMove, p->position.y + yMove, &(p->anim.GetCurrentFrame()), p->resizeRect, 1);

		App->renderer->depthBuffer[p->rect->depth].push_back(*p->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleParticles::AddParticle(const Particle& particle, int x, int y)
{
	// TODO 4: Fill in a method to create an instance of a prototype particle	
	Particle* p = new Particle(particle);
	p->position = { x, y };
	active.push_back(p);
	App->audio->PlayFx(p->fxIndex, 0);
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{}

// TODO 3: Fill in a copy constructor
Particle::Particle(const Particle& p) : anim(p.anim), position(p.position), fxIndex(p.fxIndex), speed(p.speed)
{}

Particle::~Particle()
{
	delete rect;
	delete resizeRect;
}

void Particle::Update()
{
	z += speed;
	if (z > MAX_Z) to_delete = true;

	// TODO 5: This is the core of the particle logic
	// draw and audio will be managed by ModuleParticle::Update()
	// Note: Set to_delete to true is you want it deleted
	//position.x += speed;

}

void Particle::setResizeRect(int x, int y, int w, int h)
{
	resizeRect->x = x;
	resizeRect->y = y;
	resizeRect->w = w;
	resizeRect->h = h;
}

void Particle::setRect(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, int depth)
{
	rect->texture = texture;
	rect->x = x;
	rect->y = y;
	rect->section = section;
	rect->resize = resize;
	rect->depth = depth;
}
