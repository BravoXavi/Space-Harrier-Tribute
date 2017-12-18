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
	//graphics = App->textures->Load("assets/obstacleModels.png");
	graphics = App->textures->Load("assets/metalflower.png");

	//ALIENSHIP
	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.enemyAnimation.frames.push_back({ 3, 2, 44, 44 });
	alienShip.enemyAnimation.speed = 0.1f;
	alienShip.worldPosition = { 0, 0, MAX_Z };
	alienShip.colType = ENEMY;
	alienShip.uniDimensionalSpeed = 130.0f;
	alienShip.depthSpeed = 9.0f;

	//METALFLOWER
	tomos.enemyAnimation.frames.push_back({ 3, 2, 44, 44 });
	tomos.enemyAnimation.frames.push_back({ 53, 2, 44, 46 });
	tomos.enemyAnimation.frames.push_back({ 104, 2, 44, 46 });
	tomos.enemyAnimation.frames.push_back({ 1, 53, 56, 56 });
	tomos.enemyAnimation.frames.push_back({ 64, 53, 56, 56 });
	tomos.enemyAnimation.frames.push_back({ 1, 116, 64, 68 });
	tomos.enemyAnimation.frames.push_back({ 78, 116, 64, 68 });
	tomos.enemyAnimation.speed = 0.1f;
	tomos.worldPosition = { 0, 0, MAX_Z };
	tomos.colType = ENEMY;
	tomos.uniDimensionalSpeed = 130.0f;
	tomos.depthSpeed = 3.0f;
	tomos.enemyAnimation.loop = false;

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
		
	if (active.empty()) aliveEnemy = false;

	return UPDATE_CONTINUE;
}

void ModuleEnemy::AddEnemy(const Enemy& enemy, float x, float y, float z, collisionType type, int moveSet)
{	
	fPoint pos = { x, y, z };
	Enemy* e = enemy.createEnemyInstance(enemy, pos);	
	e->colType = type;
	e->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, e->colType, (int)e->worldPosition.z, App->enemies);
	e->moveSet = moveSet;
	active.push_back(e);

	aliveEnemy = true;
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
				if ((*it)->invulnerable)
				{
					LOG("INVULNERABLE STATE!");
				}
				else
				{
					(*it)->collider->to_delete = true;
					(*it)->to_delete = true;
					App->particles->AddParticle(App->particles->explosion, (*it)->screenPosition.x, (*it)->screenPosition.y, (*it)->screenPosition.z, EXPLOSION);
					LOG("ENEMY HIT!! ENEMY DESTROYED!!");
				}			
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
				AddEnemy(alienShip, (float)-alienShip.enemyAnimation.GetCurrentFrame().w, (float)SCREEN_HEIGHT / 3.0f, 17.0f, ENEMY, 1);
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
						AddEnemy(alienShip,  3.0f * ((float)SCREEN_WIDTH / 4.0f), (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 17.0f, ENEMY, 2);
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
				if( enemyWaveCount < 5 ) AddEnemy(alienShip, (float)-alienShip.enemyAnimation.GetCurrentFrame().w, (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 3);
				else AddEnemy(alienShip, (float)(SCREEN_WIDTH + alienShip.enemyAnimation.GetCurrentFrame().w), (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 3);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}
			break;
		case 4:
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1);
			App->enemies->AddEnemy(App->enemies->tomos, (2.0f*M_PI) / 3.0f, 0.0f, 20.0f, ENEMY, 1);
			App->enemies->AddEnemy(App->enemies->tomos, (4.0f*M_PI) / 3.0f, 0.0f, 20.0f, ENEMY, 1);
			triggerEnemies = false;
			break;
		case 5:
			if (enemyWaveCount < 7)
			{
				AddEnemy(alienShip, (float)-alienShip.enemyAnimation.GetCurrentFrame().w, ((3.0f * (float)SCREEN_HEIGHT) / 4.0f) - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 4);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}
			break;
		case 6:
			if (enemyWaveCount < 6)
			{
				AddEnemy(alienShip, (float)-alienShip.enemyAnimation.GetCurrentFrame().w, (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 5);
				AddEnemy(alienShip, (float)(SCREEN_WIDTH + alienShip.enemyAnimation.GetCurrentFrame().w), (float)SCREEN_HEIGHT - ((float)FLOOR_Y_MIN / 2.0f), 2.0f, ENEMY, 5);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				triggerEnemies = false;
			}
			break;
		case 7:
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1);
			App->enemies->AddEnemy(App->enemies->tomos, (2.0f*M_PI) / 3.0f, 0.0f, 20.0f, ENEMY, 1);
			App->enemies->AddEnemy(App->enemies->tomos, (4.0f*M_PI) / 3.0f, 0.0f, 20.0f, ENEMY, 1);
			triggerEnemies = false;
			break;
		case 8:
			LOG("DRAGON BOSS...");
			break;
		default:
			break;
	}
}