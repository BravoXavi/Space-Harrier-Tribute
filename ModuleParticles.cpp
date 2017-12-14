#include <math.h>
#include "ModuleParticles.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModulePlayer.h"
#include "SDL/include/SDL_timer.h"

ModuleParticles::ModuleParticles()
{}

ModuleParticles::~ModuleParticles()
{}

// Load assets
bool ModuleParticles::Start()
{
	LOG("Loading particles");
	//graphics = App->textures->Load("assets/Shoots.png");
	graphics = App->textures->Load("assets/enemyshot.png");

	p_laser.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	p_laser.anim.frames.push_back({ 1, 1, 91, 61 });
	p_laser.anim.frames.push_back({ 95, 0, 91, 61 });
	p_laser.anim.frames.push_back({ 188, 1, 91, 61 });
	p_laser.anim.frames.push_back({ 284, 0, 91, 61 });
	p_laser.anim.speed = 0.1f;
	p_laser.position.z = 1;
	p_laser.speed = 1;
	p_laser.colType = P_LASER;

	//e_laser.fxIndex = App->audio->LoadFx("Insert audio path here");
	e_laser.anim.frames.push_back({ 10, 21, 68, 45});
	e_laser.anim.frames.push_back({ 93, 17, 62, 53 });
	e_laser.anim.frames.push_back({ 177, 13, 54, 61 });
	e_laser.anim.frames.push_back({ 261, 10, 46, 68 });
	e_laser.anim.frames.push_back({ 21, 90, 46, 68 });
	e_laser.anim.frames.push_back({ 97, 93, 54, 61 });
	e_laser.anim.frames.push_back({ 173, 98, 62, 53 });
	e_laser.anim.frames.push_back({ 250, 101, 68, 45 });
	e_laser.anim.speed = 0.1f;
	e_laser.position.z = MAX_Z;
	e_laser.speed = -0.4f;
	e_laser.colType = E_LASER;

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

		if(p->colType == P_LASER) p->p_laser_Update();
		else if (p->colType == E_LASER) p->e_laser_Update();
		else if (p->colType == EXPLOSION) p->explosion_Update();

		App->renderer->depthBuffer[(int)p->rect->z].push_back(*p->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleParticles::AddParticle(const Particle& particle, float x, float y, float z, collisionType colType)
{
	Particle* p = new Particle(particle);
	p->position = { x, y, z };
	p->colType = colType;

	if (colType == E_LASER)
	{
		p->position.y += (-App->renderer->horizonY + (float)FLOOR_Y_MIN);
		p->targetOffset.x = (App->player->position.x + (float)App->player->current_animation->GetCurrentFrame().w/2.0f) - p->position.x;
		p->targetOffset.y = (App->player->position.y + (float)App->player->current_animation->GetCurrentFrame().h/2.0f) - p->position.y;
		p->targetOffset.z = z;
	}

	if( colType == P_LASER || colType == E_LASER) p->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)z, App->particles);
	active.push_back(p);
	App->audio->PlayFx(p->fxIndex, 0);
}

bool ModuleParticles::onCollision(Collider* c1, Collider* c2)
{
	for (std::list<Particle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == c1 || (*it)->collider == c2)
		{
			if (c1->colType == D_OBSTACLE || c2->colType == D_OBSTACLE || c1->colType == NOLETHAL_D_OBSTACLE || c2->colType == NOLETHAL_D_OBSTACLE || c1->colType == ENEMY || c2->colType == ENEMY)
			{
				(*it)->to_delete = true;
				(*it)->collider->to_delete = true;
				LOG("OBSTACLE/ENEMY GETS DESTROYED AND BULLET TOO");
			}
			else if (c1->colType == ND_OBSTACLE || c2->colType == ND_OBSTACLE)
			{
				LOG("OBSTACLE REMAINS UNAFFECTED AND BULLET BOUNCES");
			}
		}
	}

	return true;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{}

Particle::Particle(const Particle& p) : anim(p.anim), position(p.position), fxIndex(p.fxIndex), speed(p.speed)
{}

Particle::~Particle()
{
	delete rect;
	delete resizeRect;
}

void Particle::p_laser_Update()
{
	if (position.z > MAX_Z)
	{
		to_delete = true;
		if (collider != nullptr) collider->to_delete = true;
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = (float)anim.GetCurrentFrame().w * zModifier;
	float newHeight = (float)anim.GetCurrentFrame().h * zModifier;
	float newX = position.x + (((float)anim.GetCurrentFrame().w - newWidth) / 2.0f);
	float newY = position.y + (((float)anim.GetCurrentFrame().h - newHeight) / 2.0f);

	newWidth *= 0.6f;
	newHeight *= 0.6f;

	setResizeRect(newWidth, newHeight);
	setRect(App->particles->graphics, newX, newY, position.z, &(anim.GetCurrentFrame()), resizeRect);

	if (collider != nullptr)
	{
		collider->SetPos((int)newX, (int)newY, (int)position.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}

	position.z += speed;
}

void Particle::e_laser_Update()
{
	if (position.z <= MIN_Z)
	{
		to_delete = true;
		if (collider != nullptr) collider->to_delete = true;
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = (float)anim.GetCurrentFrame().w * zModifier;
	float newHeight = (float)anim.GetCurrentFrame().h * zModifier;

	//Reescale shot (Sprite is too big)
	newHeight *= 0.5f;
	newWidth *= 0.5f;

	setResizeRect(newWidth, newHeight);

	float posModifier = 1.0f - (position.z/targetOffset.z);
	float newX = position.x - (newWidth / 2.0f) + (targetOffset.x * posModifier);
	float newY = position.y - (newHeight / 2.0f) + (targetOffset.y * posModifier);

	setRect(App->particles->graphics, newX, newY, position.z, &(anim.GetCurrentFrame()), resizeRect);

	if (collider != nullptr)
	{
		collider->SetPos((int)newX, (int)newY, (int)position.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}
		
	position.z += speed;
}

void Particle::explosion_Update()
{
	//TODO: Explosion when enemy or obstacle is destroyed
}

void Particle::setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, SDL_Rect* section, SDL_Rect* resize) const
{
	rect->x = x;
	rect->y = y;
	rect->z = z;
	rect->texture = texture;
	rect->section = section;
	rect->resize = resize;
}

void Particle::setResizeRect(const float& w, const float& h) const
{
	resizeRect->x = 0;
	resizeRect->y = 0;
	resizeRect->w = (int)w;
	resizeRect->h = (int)h;
}