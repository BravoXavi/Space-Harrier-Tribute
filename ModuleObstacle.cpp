#include <math.h>
#include "ModuleObstacle.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModuleParticles.h"
#include "ModuleShadows.h"
#include "ModulePlayer.h"

#include "SDL/include/SDL_timer.h"

ModuleObstacle::ModuleObstacle(bool active) : Module(active)
{}

ModuleObstacle::~ModuleObstacle()
{
}

// Load assets
bool ModuleObstacle::Start()
{
	LOG("Loading obstacles");
	graphics = App->textures->Load("assets/enemiesobstacles.png");

	tree.anim.frames.push_back({ 206, 378, 40, 158 });
	tree.worldPosition.z = MAX_Z;
	tree.colType = D_OBSTACLE;

	rock.anim.frames.push_back({ 192, 72, 59, 37 });
	rock.worldPosition.z = MAX_Z;
	rock.colType = D_OBSTACLE;
	rock.shadowCast = true;

	bush.anim.frames.push_back({ 193, 8, 59, 41 });
	bush.worldPosition.z = MAX_Z;
	bush.colType = NOLETHAL_D_OBSTACLE;

	return true;
}

// Unload assets
bool ModuleObstacle::CleanUp()
{
	LOG("Unloading particles");
	App->textures->Unload(graphics);

	for (list<Obstacle*>::iterator it = active.begin(); it != active.end(); ++it)
		RELEASE(*it);

	active.clear();

	return true;
}

// PreUpdate to clear up all dirty particles
update_status ModuleObstacle::PreUpdate()
{
	for (list<Obstacle*>::iterator it = active.begin(); it != active.end();)
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

// Update all obstacle logic and draw them
update_status ModuleObstacle::Update()
{
	for (list<Obstacle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		Obstacle* o = *it;

		if(!App->renderer->stopUpdating) o->Update();

		if (o->shadowCast) App->shadows->DrawShadow(o->screenPosition.x, o->screenPosition.y, o->screenPosition.z, o->dataToBlit->newWidth);
		App->renderer->depthBuffer[(int)o->dataToBlit->z].push_back(*o->dataToBlit);
	}

	return UPDATE_CONTINUE;
}

void ModuleObstacle::AddObstacle(const Obstacle& obstacle, float x, float xOffset, float y, collisionType type)
{
	Obstacle* o = new Obstacle(obstacle);
	o->worldPosition = { x, y, (float)MAX_Z};
	o->xOffset = xOffset;
	o->colType = type;
	o->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, o->colType, (int)o->worldPosition.z, App->obstacles);
	o->lineToFollow = App->renderer->nextTopLine;
	active.push_back(o);
}

const bool ModuleObstacle::onCollision(Collider* moduleOwner, Collider* otherCollider) 
{
	for (std::list<Obstacle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == moduleOwner)
		{
			if (otherCollider->colType != PLAYER)
			{				
				//Obstacle gets destroyed
				App->player->playerScore += 1000.0f;
				(*it)->to_delete = true;
				(*it)->collider->to_delete = true;
				if (otherCollider->colType != PLAYER)
				{
					App->particles->AddParticle(App->particles->explosion, (*it)->screenPosition.x, (*it)->screenPosition.y + (*it)->dataToBlit->newHeight, (*it)->screenPosition.z, EXPLOSION);
				}
			}
		}
	}

	return true;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Obstacle::Obstacle()
{}

// TODO 3: Fill in a copy constructor
Obstacle::Obstacle(const Obstacle& o) : anim(o.anim), worldPosition(o.worldPosition), shadowCast(o.shadowCast)
{}

Obstacle::~Obstacle()
{
	delete dataToBlit;
}

void Obstacle::Update()
{
	if (worldPosition.z <= MIN_Z + 1)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	float tempY = ((App->renderer->renderLineValues[lineToFollow]) / (float)SCREEN_SIZE);
	float scaleValue = calculateScaleValue(tempY);

	float newWidth = (float)anim.GetCurrentFrame().w * scaleValue;
	float newHeight = (float)anim.GetCurrentFrame().h * scaleValue;

	//Minimun size to avoid strange obstacle sizes
	if (newHeight < 2.0f) newHeight = 2.0f;
	if (newWidth < 1.0f) newWidth = 1.0f;
	
	//Set new projected position values for the obstacle (The projection is added by the scaleValue)
	worldPosition.z = ((float)SCREEN_HEIGHT - tempY) / (App->renderer->horizonY / (float)MAX_Z);

	xOffset -= App->renderer->playerSpeed;
	screenPosition.x = worldPosition.x - (newWidth / 2.0f) + (xOffset * scaleValue);
	screenPosition.y = tempY - newHeight - (worldPosition.y * scaleValue);
	screenPosition.z = worldPosition.z;
	
	setRect(App->obstacles->graphics, screenPosition.x, screenPosition.y, screenPosition.z, newWidth, newHeight, &(anim.GetCurrentFrame()));
	collider->SetPos((int)screenPosition.x, (int)screenPosition.y, (int)worldPosition.z, (int)newWidth, (int)newHeight);
}

float Obstacle::calculateScaleValue(float yRender)
{
	float min = (float)SCREEN_HEIGHT - App->renderer->horizonY;
	float max = (float)SCREEN_HEIGHT;
	float toReturn = (yRender - min) / (max - min);
	if (toReturn < 0.0f) toReturn = 0.0f;

	return toReturn;
}

void Obstacle::setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, const float& newWidth, const float& newHeight, SDL_Rect* section) const
{
	dataToBlit->x = x;
	dataToBlit->y = y;
	dataToBlit->z = z;
	dataToBlit->newWidth = newWidth;
	dataToBlit->newHeight = newHeight;
	dataToBlit->texture = texture;
	dataToBlit->section = section;
}
