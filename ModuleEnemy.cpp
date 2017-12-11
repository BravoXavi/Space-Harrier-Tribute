#include <math.h>
#include "ModuleEnemy.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"

#include "SDL/include/SDL_timer.h"

ModuleEnemy::ModuleEnemy()
{}

ModuleEnemy::~ModuleEnemy()
{
	//delete resizeParticleRect;
	//delete particleRect;
}

// Load assets
bool ModuleEnemy::Start()
{
	LOG("Loading enemies");
	graphics = App->textures->Load("assets/obstacleModels.png");

	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.anim.frames.push_back({ 198, 127, 46, 30 });
	alienShip.anim.speed = 0.1f;
	alienShip.z = MAX_Z;
	alienShip.speed = 1;
	alienShip.colType = ENEMY;

	return true;
}

// Unload assets
bool ModuleEnemy::CleanUp()
{
	LOG("Unloading particles");
	App->textures->Unload(graphics);

	for (list<Enemy*>::iterator it = active.begin(); it != active.end(); ++it)
		RELEASE(*it);

	active.clear();

	return true;
}

update_status ModuleEnemy::PreUpdate()
{
	for (list<Enemy*>::iterator it = active.begin(); it != active.end();)
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

update_status ModuleEnemy::Update()
{
	for (list<Enemy*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		Enemy* e = *it;

		e->Update();

		App->renderer->depthBuffer[e->rect->depth].push_back(*e->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleEnemy::AddEnemy(const Enemy& enemy, float x, float y, collisionType type)
{	
	Enemy* e = new Enemy(enemy);
	e->position = { x, y };
	e->colType = type;
	e->z = MAX_Z;
	e->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, e->colType, e->z, App->enemies);
	active.push_back(e);
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Enemy::Enemy()
{}

// TODO 3: Fill in a copy constructor
Enemy::Enemy(const Enemy& p) : anim(p.anim), position(p.position), fxIndex(p.fxIndex), speed(p.speed)
{}

Enemy::~Enemy()
{
	delete rect;
}

void Enemy::Update()
{
	//z += speed;
	if (z > MAX_Z) to_delete = true;
	
	if (attackCharged == 200)
	{
		attackCharged = 0;
		App->particles->AddParticle(App->particles->e_laser, position.x, position.y, E_LASER, z);
	}
	else attackCharged++;

	if (collider != nullptr)
	{
		collider->SetPos(position.x, position.y, z);
		collider->SetSize(anim.GetCurrentFrame().w, anim.GetCurrentFrame().h);
	}
	
	setRect(App->enemies->graphics, position.x, position.y, &(anim.GetCurrentFrame()), nullptr, z);
}

void Enemy::setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth)
{
	rect->texture = texture;
	rect->x = x;
	rect->y = y;
	rect->section = section;
	rect->resize = resize;
	rect->depth = depth;
}