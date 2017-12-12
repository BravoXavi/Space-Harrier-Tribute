#include <math.h>
#include "ModuleObstacle.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"

#include "SDL/include/SDL_timer.h"

ModuleObstacle::ModuleObstacle()
{}

ModuleObstacle::~ModuleObstacle()
{
}

// Load assets
bool ModuleObstacle::Start()
{
	LOG("Loading obstacles");
	graphics = App->textures->Load("assets/Arboles.png");
	models = App->textures->Load("assets/obstacleModels.png");

	tree.anim.frames.push_back({ 208, 50, 40, 158 });
	tree.position.z = MAX_Z;
	tree.colType = D_OBSTACLE;

	rock.anim.frames.push_back({ 192, 72, 59, 37 });
	rock.position.z = MAX_Z;
	rock.colType = D_OBSTACLE;

	bush.anim.frames.push_back({ 193, 8, 59, 41 });
	bush.position.z = MAX_Z;
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

		o->Update();

		App->renderer->depthBuffer[(int)o->rect->z].push_back(*o->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleObstacle::AddObstacle(const Obstacle& obstacle, float x, float xOffset, float y, collisionType type)
{
	Obstacle* o = new Obstacle(obstacle);
	o->position = { x, y, (float)MAX_Z};
	o->xOffset = xOffset;
	o->colType = type;
	o->collider = App->collision->AddCollider({ 0, 0, 0, 0 }, o->colType, (int)o->position.z, App->obstacles);
	o->lineToFollow = App->renderer->nextTopLine;
	active.push_back(o);
}

bool ModuleObstacle::onCollision(Collider* c1, Collider* c2)
{
	for (std::list<Obstacle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		if ((*it)->collider == c1 || (*it)->collider == c2)
		{
			(*it)->to_delete = true;
			(*it)->collider->to_delete = true;
		}
	}

	return true;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Obstacle::Obstacle()
{}

// TODO 3: Fill in a copy constructor
Obstacle::Obstacle(const Obstacle& o) : anim(o.anim), position(o.position)
{}

Obstacle::~Obstacle()
{
	delete rect;
	delete resizeRect;
}

void Obstacle::Update()
{
	if (position.z <= MIN_Z)
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
	xOffset -= App->renderer->playerSpeed;
	float newX = position.x - (newWidth / 2.0f) + (xOffset * scaleValue);
	float newY = tempY - newHeight - (position.y * scaleValue);
	position.z = ((float)SCREEN_HEIGHT - tempY) / (App->renderer->horizonY / (float)MAX_Z);

	collider->SetPos((int)newX, (int)newY, (int)position.z);
	collider->SetSize((int)newWidth, (int)newHeight);

	setResizeRect(newWidth, newHeight);
	setRect(App->obstacles->models, newX, newY, position.z, &(anim.GetCurrentFrame()), resizeRect);
}

float Obstacle::calculateScaleValue(float yRender)
{
	float min = (float)SCREEN_HEIGHT - App->renderer->horizonY;
	float max = (float)SCREEN_HEIGHT;
	float toReturn = (yRender - min) / (max - min);
	if (toReturn < 0.0f) toReturn = 0.0f;

	return toReturn;
}

void Obstacle::setResizeRect(const float& w, const float& h) const
{
	resizeRect->x = 0;
	resizeRect->y = 0;
	resizeRect->w = (int)w;
	resizeRect->h = (int)h;
}

void Obstacle::setRect(SDL_Texture* texture, const float& x, const float& y, const float& z, SDL_Rect* section, SDL_Rect* resize) const
{
	rect->x = x;
	rect->y = y;
	rect->z = z;
	rect->texture = texture;	
	rect->section = section;
	rect->resize = resize;	
}
