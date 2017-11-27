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

	tree.anim.frames.push_back({ 155, 85, 31, 122 });
	tree.z = MAX_Z;
	tree.colType = OBSTACLE;

	//Create rock obstacle
	//Create tower obstacle

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

// Update all particle logic and draw them
update_status ModuleObstacle::Update()
{
	for (list<Obstacle*>::iterator it = active.begin(); it != active.end(); ++it)
	{
		Obstacle* o = *it;

		o->Update();

		App->renderer->depthBuffer[o->rect->depth].push_back(*o->rect);
	}

	return UPDATE_CONTINUE;
}

void ModuleObstacle::AddObstacle(const Obstacle& obstacle, float x, float y)
{
	// TODO 4: Fill in a method to create an instance of a prototype particle	
	Obstacle* o = new Obstacle(obstacle);
	o->position = { x, y };
	o->lineToFollow = App->renderer->nextTopLine;
	active.push_back(o);
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Obstacle::Obstacle()
{}

// TODO 3: Fill in a copy constructor
Obstacle::Obstacle(const Obstacle& o) : anim(o.anim), position(o.position), z(o.z)
{}

Obstacle::~Obstacle()
{
	delete rect;
	delete resizeRect;
}

void Obstacle::Update()
{
	//z -= 1;
	//if (z < 1)
	//{
	//	to_delete = true;
	//}

	//Calculate Y of the obstacle
	float newY = (((float)App->renderer->renderLineValues[lineToFollow]) / (float)SCREEN_SIZE);

	//Calculate Z of the obstacle
	float zTemp = (float)SCREEN_HEIGHT - newY;
	z = (int)(zTemp / ((float)App->renderer->horizonY / (float)MAX_Z));

	//Calculate the scale of the projection
	float min = ((float)SCREEN_HEIGHT - App->renderer->horizonY);
	float max = (float)SCREEN_HEIGHT;
	float scaleValue = (newY - min) / (max - min);
	if (scaleValue < 0.0f) scaleValue = 0.0f;
	LOG("NEW UPDATE --------------------");
	LOG("newY = %f", newY);
	LOG("min = %f", min);
	LOG("max = %f", max);
	LOG("scaleValue =  %f", scaleValue);

	//Calculate X projection using the scale and playerSpeed
	//position.x -= App->renderer->playerSpeed;
	float newX = 0.0f;
	if (position.x < (float)SCREEN_WIDTH / 2.0f) newX = ((float)SCREEN_WIDTH / 2.0f) - (position.x*scaleValue);
	else newX = (position.x*scaleValue);

	//Rescale size of the obstacle and calculate other small values
	int newWidth = (int)((float)anim.GetCurrentFrame().w * scaleValue);
	int newHeight = (int)((float)anim.GetCurrentFrame().h * scaleValue);

	if (newHeight < 3) newHeight = 2;
	if (newWidth < 1) newWidth = 1;
	newY -= newHeight;

	setResizeRect(0, 0, newWidth, newHeight);
	setRect(App->obstacles->graphics, newX, newY, &(anim.GetCurrentFrame()), resizeRect, z);
}

void Obstacle::setResizeRect(int x, int y, int w, int h)
{
	resizeRect->x = x;
	resizeRect->y = y;
	resizeRect->w = w;
	resizeRect->h = h;
}

void Obstacle::setRect(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, int depth)
{
	rect->texture = texture;
	rect->x = x;
	rect->y = y;
	rect->section = section;
	rect->resize = resize;
	rect->depth = depth;
}
