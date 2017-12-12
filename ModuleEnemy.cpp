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
{}

// Load assets
bool ModuleEnemy::Start()
{
	LOG("Loading enemies");
	graphics = App->textures->Load("assets/obstacleModels.png");

	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.anim.frames.push_back({ 198, 127, 46, 30 });
	alienShip.anim.speed = 0.1f;
	alienShip.position.z = MAX_Z;
	alienShip.speed = 1;
	alienShip.colType = ENEMY;

	return true;
}

// Unload assets
bool ModuleEnemy::CleanUp()
{
	LOG("Unloading enemies");
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

		App->renderer->depthBuffer[(int)e->rect->z].push_back(*e->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleEnemy::AddEnemy(const Enemy& enemy, float x, float y, float z, collisionType type, int moveSet)
{	
	Enemy* e = new Enemy(enemy);
	e->position = { x, y, z };
	e->colType = type;
	e->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, e->colType, (int)e->position.z, App->enemies);
	active.push_back(e);
}

bool ModuleEnemy::onCollision(Collider* c1, Collider* c2)
{
	for (std::list<Enemy*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == c1 || (*it)->collider == c2)
		{
			if (c1->colType == PLAYER || c2->colType == PLAYER)
			{
				LOG("ENEMY COLLIDED WITH PLAYER - PLAYER LOSES ONE LIVE");
			}
			else if (c1->colType == P_LASER || c2->colType == P_LASER)
			{
				(*it)->collider->to_delete = true;
				(*it)->to_delete = true;
				LOG("ENEMY HIT!! ENEMY DESTROYED!!");
			}
		}
	}

	return true;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Enemy::Enemy()
{}

Enemy::Enemy(const Enemy& p) : anim(p.anim), position(p.position), fxIndex(p.fxIndex), speed(p.speed)
{}

Enemy::~Enemy()
{
	delete rect;
	delete resizeRect;
}

void Enemy::Update()
{
	if (position.z > MAX_Z) to_delete = true;
	
	if (attackCharged == 200)
	{
		attackCharged = 0;
		App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
	}
	else attackCharged++;

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = anim.GetCurrentFrame().w * zModifier;
	float newHeight = anim.GetCurrentFrame().h * zModifier;
	float newX = position.x - newWidth/2.0f;
	float newY = position.y - newHeight/2.0f;

	if (collider != nullptr)
	{
		collider->SetPos((int)newX, (int)newY, (int)position.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}
	
	setResizeRect(newWidth, newHeight);
	setRect(App->enemies->graphics, newX, newY, position.z, &(anim.GetCurrentFrame()), resizeRect);
}

void Enemy::setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, SDL_Rect* section, SDL_Rect* resize) const
{
	rect->x = x;
	rect->y = y;
	rect->z = z;
	rect->texture = texture;	
	rect->section = section;
	rect->resize = resize;
}

void Enemy::setResizeRect(const float& w, const float& h) const
{
	resizeRect->x = 0;
	resizeRect->y = 0;
	resizeRect->w = (int)w;
	resizeRect->h = (int)h;
}