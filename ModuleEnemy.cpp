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
#include "ModuleShadows.h"
#include "ModulePlayer.h"
#include "SDL/include/SDL_timer.h"

ModuleEnemy::ModuleEnemy(bool active) : Module(active)
{}

ModuleEnemy::~ModuleEnemy()
{}

// Load assets
bool ModuleEnemy::Start()
{
	LOG("Loading enemies");
	graphics = App->textures->Load("assets/enemiesobstacles.png");

	//ALIENSHIP
	alienShip.enemyAnimation.frames.push_back({ 198, 127, 46, 30 });
	alienShip.enemyAnimation.speed = 0.1f;
	alienShip.worldPosition = { 0, 0, MAX_Z };
	alienShip.colType = ENEMY;
	alienShip.uniDimensionalSpeed = 130.0f;
	alienShip.depthSpeed = 9.0f;

	//METALFLOWER
	tomos.enemyAnimation.frames.push_back({ 7, 228, 44, 44 });
	tomos.enemyAnimation.frames.push_back({ 57, 228, 46, 46 });
	tomos.enemyAnimation.frames.push_back({ 108, 228, 46, 46 });
	tomos.enemyAnimation.frames.push_back({ 5, 279, 56, 56 });
	tomos.enemyAnimation.frames.push_back({ 68, 279, 56, 56 });
	tomos.enemyAnimation.frames.push_back({ 5, 342, 64, 68 });
	tomos.enemyAnimation.frames.push_back({ 82, 342, 64, 68 });
	tomos.enemyAnimation.speed = 3.0f;
	tomos.worldPosition = { 0, 0, MAX_Z };
	tomos.colType = ENEMY;
	tomos.uniDimensionalSpeed = 130.0f;
	tomos.depthSpeed = 3.0f;
	tomos.enemyAnimation.loop = false;

	//DRAGON - Head
	dragonHead.enemyAnimation.frames.push_back({ 347, 49, 65, 104 });
	dragonHead.enemyAnimation.frames.push_back({ 268, 48, 69, 108 });

	dragonHead.enemyAnimation.speed = 0.0f;
	dragonHead.worldPosition = { 0, 0, MAX_Z };
	dragonHead.colType = ENEMY;
	dragonHead.uniDimensionalSpeed = 130.0f;
	dragonHead.depthSpeed = -6.0f;
	dragonHead.enemyAnimation.loop = false;
	dragonHead.lifePoints = 8;

	//DRAGON - Body
	dragonBody.enemyAnimation.frames.push_back({ 429, 68, 111, 71 });
	dragonBody.enemyAnimation.speed = 0.0f;
	dragonBody.colType = ND_ENEMY;
	dragonBody.uniDimensionalSpeed = 130.0f;
	dragonBody.depthSpeed = -6.0f;
	dragonBody.worldPosition = { 0, 0, MAX_Z };
	dragonBody.colType = ENEMY;

	//DRAGON - Tail
	dragonTail.enemyAnimation.frames.push_back({ 552, 60, 94, 89 });
	dragonTail.enemyAnimation.speed = 0.0f;
	dragonTail.colType = ND_ENEMY;
	dragonTail.uniDimensionalSpeed = 130.0f;
	dragonTail.depthSpeed = -6.0f;
	dragonTail.worldPosition = { 0, 0, MAX_Z };
	dragonTail.colType = ENEMY;

	spawnTimer = 0;
	enemyWaveCount = 0;
	waveNum = 0;
	triggerEnemies = false;
	bossEncounter = false;
	aliveEnemy = false;

	spaceshipSFX = App->audio->LoadFx("assets/sfx/ENEMY_Alienship.wav");
	tomosSFX = App->audio->LoadFx("assets/sfx/ENEMY_Tomos.wav");

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

		if(!App->player->gotHit) e->Update();

		App->renderer->depthBuffer[(int)e->dataToBlit->z].push_back(*e->dataToBlit);
		App->shadows->DrawShadow(e->screenPosition.x, 0, e->screenPosition.z, e->dataToBlit->newWidth);
	}
		
	if (active.empty()) aliveEnemy = false;

	return UPDATE_CONTINUE;
}

void ModuleEnemy::AddEnemy(const Enemy& enemy, float x, float y, float z, collisionType type, int moveSet, const float& oscillationAngle)
{	
	fPoint pos = { x, y, z };
	Enemy* e = enemy.createEnemyInstance(enemy, pos, type, moveSet, oscillationAngle);	
	active.push_back(e);

	aliveEnemy = true;
}

void ModuleEnemy::AddModularEnemy(const Enemy& head, const Enemy& body, const Enemy& tail, float x, float y, float z, const int& moveSet, const int& bodySize, const float& oscillationAngle)
{
	fPoint pos = { x, y, z };
	Enemy* h = head.createEnemyInstance(head, pos, ENEMY, moveSet, oscillationAngle);
	active.push_back(h);

	Enemy* previous = h;
	Enemy* b = nullptr;
	for (int i = 1; i <= bodySize; i++)
	{
		pos.z = z + (i+2);
		b = body.createEnemyInstance(body, pos, ND_ENEMY, moveSet+1, previous->oscillationAngle - 0.1f);
		b->superiorBodyPart = previous;
		b->lifePoints = h->lifePoints;
		active.push_back(b);

		previous = b;
	}

	pos.z = z + (bodySize + 3);
	Enemy* t = tail.createEnemyInstance(tail, pos, ND_ENEMY, moveSet+1, previous->oscillationAngle - 0.1f);
	t->superiorBodyPart = previous;
	t->lifePoints = h->lifePoints;
	active.push_back(t);

	aliveEnemy = true;
}

bool ModuleEnemy::onCollision(Collider* moduleOwner, Collider* otherCollider)
{
	for (std::list<Enemy*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == moduleOwner)
		{
			if (otherCollider->colType == PLAYER)
			{
				LOG("ENEMY COLLIDED WITH PLAYER - PLAYER LOSES ONE LIVE");
			}
			else if (otherCollider->colType == P_LASER)
			{
				if (moduleOwner->colType != ND_ENEMY)
				{
					(*it)->lifePoints -= 1;
					if ((*it)->lifePoints <= 0)
					{
						App->player->playerScore += 2000.0f;
						//Enemy is dead
						(*it)->collider->to_delete = true;
						(*it)->to_delete = true;
						App->particles->AddParticle(App->particles->explosion, (*it)->screenPosition.x, (*it)->screenPosition.y + (*it)->dataToBlit->newHeight, (*it)->screenPosition.z, EXPLOSION);
					}
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
			if(enemyWaveCount == 0) App->audio->PlayFx(spaceshipSFX);
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
			if (enemyWaveCount == 0) App->audio->PlayFx(spaceshipSFX);
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
			if (enemyWaveCount == 0) App->audio->PlayFx(spaceshipSFX);
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
			if (enemyWaveCount == 0) App->audio->PlayFx(tomosSFX);
			//METALFLOWER wave needs a certain oscillation angle for some of the enemies
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, 0.0f);
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, (2.0f * (float)M_PI) / 3.0f);
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, (4.0f * (float)M_PI) / 3.0f);
			triggerEnemies = false;
			break;
		case 5:
			if (enemyWaveCount == 0) App->audio->PlayFx(spaceshipSFX);
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
			if (enemyWaveCount == 0) App->audio->PlayFx(spaceshipSFX);
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
			//METALFLOWER wave needs a certain oscillation angle for some of the enemies
			if (enemyWaveCount == 0) App->audio->PlayFx(tomosSFX);
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, 0.0f);
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, (2.0f * (float)M_PI) / 3.0f);
			App->enemies->AddEnemy(App->enemies->tomos, 0.0f, 0.0f, 20.0f, ENEMY, 1, (4.0f * (float)M_PI) / 3.0f);
			triggerEnemies = false;
			break;
		case 8:
			App->enemies->AddModularEnemy(App->enemies->dragonHead, App->enemies->dragonBody, App->enemies->dragonTail, 100.f, 100.0f, 15.0f, 1, 5);
			triggerEnemies = false;
			break;
		default:
			break;
	}
}