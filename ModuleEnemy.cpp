#include <math.h>
#include "ModuleEnemy.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
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
	graphics = App->textures->Load("assets/Arboles.png");

	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.anim.frames.push_back({ 155, 85, 31, 122 });
	alienShip.anim.speed = 0.1f;
	alienShip.z = MAX_Z;
	alienShip.speed = 1;
	alienShip.colType = ENEMY;
	//tree.collision = new Collider({ tree.position.x, tree.position.y, 16, 12 });

	// TODO 12: Create a new "Explosion" particle -- DONE
	// audio: rtype/explosion.wav
	// coords: {274, 296, 33, 30}; {313, 296, 33, 30}; {346, 296, 33, 30}; {382, 296, 33, 30}; {419, 296, 33, 30}; {457, 296, 33, 30};

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

// PreUpdate to clear up all dirty particles
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

// Update all particle logic and draw them
update_status ModuleEnemy::Update()
{
	for (list<Enemy*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		Enemy* e = *it;

		e->Update();

		//float zModifier = 1.0f - ((float)p->z / MAX_Z);
		//int newWidth = (int)(p->anim.GetCurrentFrame().w * zModifier);
		//int newHeight = (int)(p->anim.GetCurrentFrame().h * zModifier);
		//int xMove = (p->anim.GetCurrentFrame().w - newWidth) / 2;
		//int yMove = (p->anim.GetCurrentFrame().h - newHeight) / 2;

		//p->setResizeRect(0, 0, newWidth, newHeight);
		//p->setRect(graphics, p->position.x + xMove, p->position.y + yMove, &(p->anim.GetCurrentFrame()), p->resizeRect, 1);

		e->setRect(graphics, e->position.x, (SCREEN_HEIGHT-App->renderer->horizonY) - e->anim.GetCurrentFrame().h, &(e->anim.GetCurrentFrame()), nullptr, 1);
		App->renderer->depthBuffer[e->rect->depth].push_back(*e->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleEnemy::AddEnemy(const Enemy& enemy, int x, int y)
{
	// TODO 4: Fill in a method to create an instance of a prototype particle	
	Enemy* e = new Enemy(enemy);
	e->position = { x, y };
	active.push_back(e);
	App->audio->PlayFx(e->fxIndex, 0);
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
	//if (z > MAX_Z) to_delete = true;

	// TODO 5: This is the core of the particle logic
	// draw and audio will be managed by ModuleParticle::Update()
	// Note: Set to_delete to true is you want it deleted
	//position.x += speed;

}

void Enemy::setRect(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, int depth)
{
	rect->texture = texture;
	rect->x = x;
	rect->y = y;
	rect->section = section;
	rect->resize = resize;
	rect->depth = depth;
}