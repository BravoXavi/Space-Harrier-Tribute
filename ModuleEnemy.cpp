#include <math.h>
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

	alienShip.fxIndex = App->audio->LoadFx("rtype/laser.wav");
	alienShip.anim.frames.push_back({ 198, 127, 46, 30 });
	alienShip.anim.speed = 0.1f;
	alienShip.position.z = MAX_Z;
	alienShip.colType = ENEMY;
	alienShip.depthSpeed = 9.0f;

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
	e->moveSet = moveSet;
	e->uniDimensionalSpeed = (float)e->anim.GetCurrentFrame().w * 3.0f;
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
				AddEnemy(alienShip, (float)-alienShip.anim.GetCurrentFrame().w, (float)SCREEN_HEIGHT / 3.0f, 17.0f, ENEMY, 1);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				waveNum++;
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
				waveNum++;
				triggerEnemies = false;
			}	
			break;

		case 3:
			if (enemyWaveCount < 11)
			{
				if(enemyWaveCount < 5) AddEnemy(alienShip, (float)-alienShip.anim.GetCurrentFrame().w, (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 2.0f, ENEMY, 3);
				else AddEnemy(alienShip, (float)(SCREEN_WIDTH + alienShip.anim.GetCurrentFrame().w), (float)SCREEN_HEIGHT - (App->renderer->horizonY / 2.0f), 2.0f, ENEMY, 3);
				enemyWaveCount++;
			}
			else
			{
				enemyWaveCount = 0;
				waveNum++;
				triggerEnemies = false;
			}
			break;

		default:
			break;
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Enemy::Enemy()
{}

Enemy::Enemy(const Enemy& e) : anim(e.anim), position(e.position), fxIndex(e.fxIndex), depthSpeed(e.depthSpeed)
{}

Enemy::~Enemy()
{
	delete rect;
	delete resizeRect;
}

void Enemy::Update()
{
	if (position.z <= MIN_Z || position.z > MAX_Z)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = anim.GetCurrentFrame().w * zModifier;
	float newHeight = anim.GetCurrentFrame().h * zModifier;
	float newX = position.x - newWidth / 2.0f;
	float newY = position.y - newHeight / 2.0f;

	//Move the enemy according to its preset movement
	selectMovementPatron(moveSet);

	newY += (- App->renderer->horizonY + (float)FLOOR_Y_MIN);

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

void Enemy::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	switch (moveSelector)
	{
	case 1:
		if (position.x < (float)(SCREEN_WIDTH + anim.GetCurrentFrame().w * 2.0f) && position.y == (float)SCREEN_HEIGHT / 3.0f)
		{
			if (position.x >(float)SCREEN_WIDTH / 2.0f) position.x += deltaUniDimensionalSpeed*1.5f;
			else position.x += deltaUniDimensionalSpeed;
		}
		else
		{
			if (position.y == (float)SCREEN_HEIGHT / 3.0f) position.y = 2.0f * ((float)SCREEN_HEIGHT / 3.0f);
			position.x += -deltaUniDimensionalSpeed * 1.6f;
			position.z -= deltaDepthSpeed;
			position.y += 0.3f;
			if (position.x < -(float)anim.GetCurrentFrame().w) position.y = (float)SCREEN_HEIGHT / 3.0f;
		}
		break;

	case 2:
		if (position.y > (float)SCREEN_HEIGHT / 3.0f && position.z == 17.0f)
		{
			position.y -= deltaUniDimensionalSpeed;
		}
		else
		{
			if (position.z == 17.0f) App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
			if (position.y < (3.0f * (float)SCREEN_HEIGHT / 4.0f))
			{
				position.y += deltaUniDimensionalSpeed;
				position.z -= deltaDepthSpeed/3.0f;
			}
			else 
			{
				position.z -= deltaDepthSpeed * 1.2f;
			}
			
		}
		break;

	case 3:	
		if (position.x >= (float)(SCREEN_WIDTH + anim.GetCurrentFrame().w))
		{
			uniDimensionalSpeed *= -1;
			deltaUniDimensionalSpeed *= -1;
		}
	
		if (position.x < ((float)SCREEN_WIDTH / 5.0f) && deltaUniDimensionalSpeed < 0.0f)
		{
			uniDimensionalSpeed *= -1.0f;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}
		else if (position.x > (4.0f * ((float)SCREEN_WIDTH / 5.0f)) && deltaUniDimensionalSpeed > 0.0f)
		{
			uniDimensionalSpeed *= -1.0f;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}

		position.x += deltaUniDimensionalSpeed;
		position.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
		position.z += deltaDepthSpeed * 0.8f;
		break;

	default:
		break;
	}
}