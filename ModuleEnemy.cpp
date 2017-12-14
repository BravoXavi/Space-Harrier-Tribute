#include <math.h>
#include "Enemy.h"
#include "ModuleEnemy.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModuleTime.h"
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
	//graphics = App->textures->Load("assets/metalflower.png");

	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.idle.frames.push_back({ 198, 127, 46, 30 });
	alienShip.idle.speed = 0.1f;
	alienShip.position.z = MAX_Z;
	alienShip.colType = ENEMY;
	alienShip.uniDimensionalSpeed = 130.0f;
	alienShip.depthSpeed = 9.0f;
	alienShip.current_animation = &alienShip.idle;

	//metalFlower.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	//metalFlower.anim.frames.push_back({ 3, 2, 44, 44 });
	//metalFlower.anim.frames.push_back({ 53, 2, 44, 46 });
	//metalFlower.anim.frames.push_back({ 104, 2, 44, 46 });
	//metalFlower.anim.frames.push_back({ 1, 53, 56, 56 });
	//metalFlower.anim.frames.push_back({ 64, 53, 56, 56 });
	//metalFlower.anim.frames.push_back({ 1, 116, 64, 68 });
	//metalFlower.anim.frames.push_back({ 78, 116, 64, 68 });
	//metalFlower.anim.speed = 0.1f;
	//metalFlower.position.z = MAX_Z;
	//metalFlower.colType = ENEMY;
	//metalFlower.depthSpeed = 5.0f;

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
	Enemy* e = enemy.createEnemyInstance(enemy);
	e->position = { x, y, z };
	e->colType = type;
	e->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, e->colType, (int)e->position.z, App->enemies);
	e->moveSet = moveSet;
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

void ModuleEnemy::enemyWave(const int& selector)
{
	switch (selector)
	{
		case 1:
			if (enemyWaveCount < 6)
			{			
				AddEnemy(alienShip, (float)-alienShip.current_animation->GetCurrentFrame().w, (float)SCREEN_HEIGHT / 3.0f, 17.0f, ENEMY, 1);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}
			break;

		case 2:
			if (enemyWaveCount < 3)
			{
				switch (enemyWaveCount)
				{
					case 0:
						AddEnemy(alienShip, 3.0f * ((float)SCREEN_WIDTH / 4.0f), (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 17.0f, ENEMY, 2);
						break;
					case 1:
						AddEnemy(alienShip, 2.0f * ((float)SCREEN_WIDTH / 4.0f), (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 17.0f, ENEMY, 2);
						break;
					case 2:
						AddEnemy(alienShip, (float)SCREEN_WIDTH / 4.0f, (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 17.0f, ENEMY, 2);
						break;
				}
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}	
			break;

		case 3:
			if (enemyWaveCount < 11)
			{
				if(enemyWaveCount < 5) AddEnemy(alienShip, (float)-alienShip.current_animation->GetCurrentFrame().w, (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 3);
				else AddEnemy(alienShip, (float)(SCREEN_WIDTH + alienShip.current_animation->GetCurrentFrame().w), (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 3);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}
			break;

		default:
			break;
	}
}