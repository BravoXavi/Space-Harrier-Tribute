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

	tree.anim.frames.push_back({ 208, 50, 40, 158 });
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

// Update all obstacle logic and draw them
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

void ModuleObstacle::AddObstacle(const Obstacle& obstacle, float x, float xOffset, float y)
{
	Obstacle* o = new Obstacle(obstacle);
	o->position = { x, y };
	o->xOffset = xOffset;
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
	if (z <= 1)
	{
		to_delete = true;
	}

	//Calculate Y of the obstacle
	float newY = ((App->renderer->renderLineValues[lineToFollow]) / (float)SCREEN_SIZE);

	//Calculate Z of the obstacle
	z = (int)( ((float)SCREEN_HEIGHT - newY) / (App->renderer->horizonY / (float)MAX_Z) );

	//Calculate the scale of the projection
	scaleValue = calculateScaleValue(newY);

	//Calculate X projection using the scale and playerSpeed
	xOffset -= App->renderer->playerSpeed;
	float newX = position.x + (xOffset*scaleValue);

	//Rescale size of the obstacle and calculate other small values
	int newWidth = (int)((float)anim.GetCurrentFrame().w * scaleValue);
	int newHeight = (int)((float)anim.GetCurrentFrame().h * scaleValue);

	if (newHeight < 2) newHeight = 2;
	if (newWidth < 1) newWidth = 1;
	newY -= newHeight;

	setResizeRect(0, 0, newWidth, newHeight);
	setRect(App->obstacles->graphics, newX - (newWidth/2.0f), newY, &(anim.GetCurrentFrame()), resizeRect, z);
}

float Obstacle::calculateScaleValue(float yRender)
{
	float min = ((float)SCREEN_HEIGHT - App->renderer->horizonY);
	float max = (float)SCREEN_HEIGHT;
	float toReturn = (yRender - min) / (max - min);
	if (toReturn < 0.0f) toReturn = 0.0f;

	return toReturn;
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
