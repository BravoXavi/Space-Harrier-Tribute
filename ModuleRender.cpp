#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "SDL/include/SDL.h"

ModuleRender::ModuleRender()
{
	camera.x = camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
	horizonY = FLOOR_Y_MIN;

	startDistanceBetweenAlphaLines = ALPHA_DISTANCE_MIN;
	startSizeOfAlphaLines = ALPHA_SIZE_MIN;
	iterationOfAlphaLine = 0;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if(VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);
	
	if(renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleRender::PreUpdate()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	for (auto it = depthBuffer.rbegin(); it != depthBuffer.rend(); ++it)
	{
		if (!it->second.empty())
		{
			for (std::vector<BlitTarget>::iterator itt = it->second.begin(); itt != it->second.end(); ++itt)
			{
				Blit(itt->texture, itt->x, itt->y, itt->section, itt->resize);
			}
		}
	}

	depthBuffer.clear();

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_RenderPresent(renderer);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window
	if(renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, float speed)
{
	bool ret = true;
	SDL_Rect rect;

	rect.x = x * SCREEN_SIZE;
	rect.y = y * SCREEN_SIZE;

	if (resize != NULL)
	{
		rect.w = resize->w;
		rect.h = resize->h;
	}
	else if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE;
	rect.h *= SCREEN_SIZE;

	if(SDL_RenderCopy(renderer, texture, section, &rect) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Blit floor to screen ( (0,0) will be the down-mid of the screen )
bool ModuleRender::FloorBlit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed)
{
	bool ret = true;
	SDL_Rect rect;
	int textW, textH;
	SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

	rect.w = SCREEN_WIDTH;
	rect.h = horizonY;

	int pX = (SCREEN_WIDTH / 2) + x;
	int pY = SCREEN_HEIGHT - y;
	pX = pX - (rect.w / 2);
	pY = pY - rect.h;

	rect.x = pX * SCREEN_SIZE;
	rect.y = pY * SCREEN_SIZE;
	rect.w *= SCREEN_SIZE;
	rect.h *= SCREEN_SIZE;

	int maxExtraPixelsX = ((textW - SCREEN_WIDTH) / 2);

	if (increasingExtraPixelsX >= (120.0f) || increasingExtraPixelsX <= (-120.0f))
	{
		increasingExtraPixelsX = 0.0f;
	}
	increasingExtraPixelsX += playerSpeed;

	float pixelsPerRow = (float)textH / rect.h;
	float pixelsPerRowOffset = 0.0f;

	SDL_Rect textureLine = { maxExtraPixelsX / 2, 0, textW - maxExtraPixelsX, 1 };
	rect.h = 1;

	int originalRectX = textureLine.x;
	float deviation = 0.0f;

	for (int i = 0; i <= (horizonY*SCREEN_SIZE); i++)
	{
		deviation = (((float)i / ((float)horizonY*(float)SCREEN_SIZE))*increasingExtraPixelsX);	
		textureLine.x = originalRectX + (int)deviation;
		SDL_RenderCopy(renderer, texture, &textureLine, &rect);
		pixelsPerRowOffset += pixelsPerRow;
		textureLine.y = (int)pixelsPerRowOffset;
		rect.y += 1;		
	}

	AlphaVerticalLinesMove();

	return ret;
}

void ModuleRender::AlphaVerticalLinesMove()
{	
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	distanceBetweenAlphaLines = startDistanceBetweenAlphaLines;
	sizeOfAlphaLines = startSizeOfAlphaLines;

	float offsetDif = 0;
	float normalIteration = (float)iterationOfAlphaLine*((startDistanceBetweenAlphaLines + startSizeOfAlphaLines) / 10.0f);
	int tempIteration = (int)normalIteration % (int)(startDistanceBetweenAlphaLines * 2);
	
	float coef = tempIteration / distanceBetweenAlphaLines;
	if (coef >= 1.8f) 
	{
		iterationOfAlphaLine = 0;
	}
			
	while(distanceBetweenAlphaLines <= horizonY*SCREEN_SIZE)
	{
		SDL_Rect test = { 0, SCREEN_HEIGHT * SCREEN_SIZE - (int)(distanceBetweenAlphaLines - (coef*sizeOfAlphaLines)), SCREEN_WIDTH * SCREEN_SIZE, (int)(sizeOfAlphaLines + (offsetDif * (coef / 2.0f))) };
		DrawQuad(test, 0, 100, 0, 100, false);

		offsetDif = sizeOfAlphaLines / 4.0f;
		sizeOfAlphaLines -= offsetDif;
		
		if (sizeOfAlphaLines <= 1) {
			sizeOfAlphaLines = 1;
		}
		distanceBetweenAlphaLines += (sizeOfAlphaLines * 2.0f);
	}

	iterationOfAlphaLine += 1;
}

void ModuleRender::ModifyFloorLines(float percent) {
	startDistanceBetweenAlphaLines = ALPHA_DISTANCE_MIN + (percent*(ALPHA_DISTANCE_MAX - ALPHA_DISTANCE_MIN));
	startSizeOfAlphaLines = ALPHA_SIZE_MIN + (percent*(ALPHA_SIZE_MAX - ALPHA_SIZE_MIN));	
}

bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * SCREEN_SIZE);
		rec.y = (int)(camera.y + rect.y * SCREEN_SIZE);
		rec.w *= SCREEN_SIZE;
		rec.h *= SCREEN_SIZE;
	}

	if (SDL_RenderFillRect(renderer, &rec) != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}