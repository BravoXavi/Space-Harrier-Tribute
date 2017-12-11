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
	alienShip.position.z = MAX_Z;
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

void ModuleEnemy::AddEnemy(const Enemy& enemy, float x, float y, collisionType type, float depth)
{	
	Enemy* e = new Enemy(enemy);
	e->position = { x, y, depth};
	e->colType = type;
	e->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, e->colType, e->position.z, App->enemies);
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

// TODO 3: Fill in a copy constructor
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
		App->particles->AddParticle(App->particles->e_laser, position.x, position.y, E_LASER, position.z);
	}
	else attackCharged++;

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	int newWidth = (int)(anim.GetCurrentFrame().w * zModifier);
	int newHeight = (int)(anim.GetCurrentFrame().h * zModifier);
	int xOffset = newWidth/2;
	int yOffset = newHeight/2;

	if (collider != nullptr)
	{
		collider->SetPos(position.x - xOffset, position.y - yOffset, position.z);
		collider->SetSize(newWidth, newHeight);
	}
	
	setResizeRect(0, 0, newWidth, newHeight);
	setRect(App->enemies->graphics, position.x - xOffset, position.y - yOffset, &(anim.GetCurrentFrame()), resizeRect, position.z);
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

void Enemy::setResizeRect(int x, int y, int w, int h)
{
	resizeRect->x = x;
	resizeRect->y = y;
	resizeRect->w = w;
	resizeRect->h = h;
}