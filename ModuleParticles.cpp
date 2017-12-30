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
	//Texture
	graphics = App->textures->Load("assets/particle_models.png");

	//Sounds
	repelledShotSFX = App->audio->LoadFx("assets/sfx/SFX_RepelledBullet.wav");

	//PlayerLaser particle (Main attack)
	p_laser.fxIndex = App->audio->LoadFx("assets/sfx/PLAYER_Shot.wav");
	p_laser.anim.frames.push_back({ 1, 1, 91, 61 });
	p_laser.anim.frames.push_back({ 95, 0, 91, 61 });
	p_laser.anim.frames.push_back({ 188, 1, 91, 61 });
	p_laser.anim.frames.push_back({ 284, 0, 91, 61 });
	p_laser.anim.speed = 8.0f;
	p_laser.position.z = 1.0f;
	p_laser.speed = 1.0f;
	p_laser.colType = P_LASER;

	//EnemyLaser particle. The behaviour is the same as PlayerLaser but reversed and with a certain target
	e_laser.anim.frames.push_back({ 34, 105, 68, 45});
	e_laser.anim.frames.push_back({ 117, 101, 62, 53 });
	e_laser.anim.frames.push_back({ 201, 97, 54, 61 });
	e_laser.anim.frames.push_back({ 285, 94, 46, 68 });
	e_laser.anim.frames.push_back({ 45, 174, 46, 68 });
	e_laser.anim.frames.push_back({ 121, 177, 54, 61 });
	e_laser.anim.frames.push_back({ 197, 182, 62, 53 });
	e_laser.anim.frames.push_back({ 274, 185, 68, 45 });
	e_laser.anim.speed = 8.0f;
	e_laser.position.z = MAX_Z;
	e_laser.speed = -0.4f;
	e_laser.colType = E_LASER;

	//FireBall particle has the same behaviour as EnemyLaser particle
	fireBall.anim.frames.push_back({ 6, 280, 75, 69 });
	fireBall.anim.frames.push_back({ 90, 282, 71, 64 });
	fireBall.anim.frames.push_back({ 172, 278, 74, 71 });
	fireBall.anim.speed = 8.0f;
	fireBall.position.z = MAX_Z;
	fireBall.speed = -0.2f;
	fireBall.colType = E_LASER;

	//Explosion particle works completely different from other particles (is not an attack)
	explosion.anim.frames.push_back({ 400, 5, 89, 65 });
	explosion.anim.frames.push_back({ 497, 6, 88, 64 });
	explosion.anim.frames.push_back({ 595, 6, 90, 70 });
	explosion.anim.frames.push_back({ 690, 5, 97, 75 });
	explosion.anim.frames.push_back({ 794, 9, 93, 68 });
	explosion.anim.frames.push_back({ 895, 13, 93, 65 });
	explosion.anim.frames.push_back({ 399, 85, 96, 83 });
	explosion.anim.frames.push_back({ 504, 86, 90, 82 });
	explosion.anim.frames.push_back({ 599, 92, 95, 77 });
	explosion.anim.speed = 10.0f;
	explosion.position.z = MAX_Z;
	explosion.speed = -0.5f;
	explosion.anim.loop = false;
	explosion.colType = EXPLOSION;

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

		if (p->colType == P_LASER) 
			p->Update(1);
		else if (p->colType == E_LASER) 
			p->Update(2);
		else if (p->colType == EXPLOSION) 
			p->Update(3);

		App->renderer->depthBuffer[(int)p->dataToBlit->z].push_back(*p->dataToBlit);
	}

	return UPDATE_CONTINUE;
}

void ModuleParticles::AddParticle(const Particle& particle, const float& x, const float& y, const float& z, collisionType colType)
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

	if (colType == P_LASER || colType == E_LASER) 
		p->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)z, App->particles);

	App->audio->PlayFx(p->fxIndex, 0);
	active.push_back(p);
}

const bool ModuleParticles::onCollision(Collider* moduleOwner, Collider* otherCollider)
{
	for (std::list<Particle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == moduleOwner || (*it)->collider == otherCollider)
		{
			if (otherCollider->colType == D_OBSTACLE || otherCollider->colType == NOLETHAL_D_OBSTACLE || otherCollider->colType == ENEMY)
			{
				//Bullet gets destroyed with the other object
				(*it)->to_delete = true;
				(*it)->collider->to_delete = true;
			}
			else if (otherCollider->colType == ND_ENEMY)
			{
				//Bullet gets repelled and the enemy remains unmodified
				App->audio->PlayFx(repelledShotSFX);
				(*it)->repelled = true;
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
	delete dataToBlit;
}

void Particle::Update(const int& updateSelector)
{
	LOG("Position Z: %f", position.z);
	if (position.z >= MAX_Z || position.z <= MIN_Z || anim.animationWithoutLoopEnded)
	{
		to_delete = true;
		if(collider != nullptr) 
			collider->to_delete = true;
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = (float)anim.GetCurrentFrame().w * zModifier;
	float newHeight = (float)anim.GetCurrentFrame().h * zModifier;
	float newX = 0.0f;
	float newY = 0.0f;

	newWidth *= 0.6f;
	newHeight *= 0.6f;

	if (!repelled)
	{
		//The movement behaviour is slightly different in each of the three particles, so we use a switch to choose between them
		switch (updateSelector)
		{
			case 1: //Player bullet
			{
				newX = position.x - (newWidth / 2.0f);
				newY = position.y - (newHeight / 4.0f);
				break;
			}
			case 2: //Enemy bullet
			{
				float posModifier = 1.0f - (position.z / targetOffset.z);
				newX = position.x - (newWidth / 2.0f) + (targetOffset.x * posModifier);
				newY = position.y - (newHeight / 2.0f) + (targetOffset.y * posModifier);
				break;
			}
			case 3: //Explosion
			{
				newX = position.x - (newWidth / 2.0f);
				newY = position.y - newHeight;
				break;
			}
		}
	}
	else
	{
		if (collider != nullptr)
		{
			collider->to_delete = true;
			collider = nullptr;
		}

		position.x += 3.0f;
		position.y -= 3.0f;
		newX = position.x - newWidth / 2.0f;
		newY = position.y - newHeight / 4.0f;
	}

	position.z += speed;

	setDataToBlit(App->particles->graphics, newX, newY, position.z, newWidth, newHeight, &(anim.GetCurrentFrame()));

	if (collider != nullptr)
		collider->SetPos((int)newX, (int)newY, (int)position.z, (int)newWidth, (int)newHeight);	
}

const void Particle::setDataToBlit(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const
{
	dataToBlit->x = x;
	dataToBlit->y = y;
	dataToBlit->z = z;
	dataToBlit->newWidth = newWidth;
	dataToBlit->newHeight = newHeight;
	dataToBlit->texture = texture;
	dataToBlit->section = section;	
}