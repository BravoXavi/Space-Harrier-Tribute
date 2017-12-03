#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "SDL/include/SDL.h"

const float ModuleRender::LINE_REDUCTION = 0.60f;

ModuleRender::ModuleRender()
{
	camera.x = camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
	horizonY = FLOOR_Y_MIN;
	firstLinePositionPercentage = 0.0f;
	firstLineIndex = 0;
	nextTopLine = 0;

	for (int i = 0; i < alphaLines; i++)
	{
		alphaLinesArray[i].x = 0;
		alphaLinesArray[i].w = SCREEN_WIDTH*SCREEN_SIZE;
		lineDivisor += pow(LINE_REDUCTION, i);
	}	
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
	int size = depthBuffer.size();

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

//Draw a quad on screen
bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	if (SDL_RenderFillRect(renderer, &rect) != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, float x, float y, SDL_Rect* section, SDL_Rect* resize, float speed)
{
	bool ret = true;
	SDL_Rect rect;

	rect.x = (int)(x * SCREEN_SIZE);
	rect.y = (int)(y * SCREEN_SIZE);

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

void ModuleRender::BackgroundBlit(SDL_Texture* texture)
{
	bool ret = true;
	SDL_Rect rect;
	int textW;
	int textH;
	SDL_Rect screenCutLeft;

	SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

	backgroundXOffset -= (playerSpeed*0.2);
	rect.x = 0;
	rect.y = 0;

	rect.x = backgroundXOffset;
	rect.y -= horizonY*SCREEN_SIZE;
	rect.w = textW * SCREEN_SIZE;
	rect.h = (SCREEN_HEIGHT*SCREEN_SIZE);

	screenCutLeft.x = -textW + backgroundXOffset;
	screenCutLeft.y = rect.y;
	screenCutLeft.w = rect.w;
	screenCutLeft.h = rect.h;

	if (backgroundXOffset > 0.0f && backgroundXOffset < (SCREEN_WIDTH*SCREEN_SIZE))
	{
		SDL_RenderCopy(renderer, texture, nullptr, &screenCutLeft);
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}
	else if (backgroundXOffset > (SCREEN_WIDTH*SCREEN_SIZE))
	{
		SDL_RenderCopy(renderer, texture, nullptr, &screenCutLeft);
	}
	else if (backgroundXOffset <= (-textW+SCREEN_WIDTH)*SCREEN_SIZE)
	{
		screenCutLeft.x = (textW*SCREEN_SIZE) + rect.x;
		screenCutLeft.y = rect.y;
		screenCutLeft.w = rect.w;
		screenCutLeft.h = rect.h;
		SDL_RenderCopy(renderer, texture, nullptr, &screenCutLeft);
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}
	else
	{
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}

	if (backgroundXOffset > textW || backgroundXOffset <= (-textW*SCREEN_SIZE)) backgroundXOffset = 0.0f;
	
}

// Blit floor to screen ( (0,0) will be the down-mid of the screen )
bool ModuleRender::FloorBlit(SDL_Texture* texture, SDL_Rect* section, float speed)
{
	bool ret = true;
	SDL_Rect rect;
	int textW, textH;
	SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

	rect.w = SCREEN_WIDTH;
	rect.h = (int)horizonY;

	int pX = (SCREEN_WIDTH / 2);
	int pY = SCREEN_HEIGHT;
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
	float baseLineHeight = (float)(horizonY*SCREEN_SIZE) / lineDivisor;

	float startRenderPos = (float)SCREEN_HEIGHT*(float)SCREEN_SIZE - baseLineHeight*(1.0f - firstLinePositionPercentage);
	float firstLineHeight = baseLineHeight * (1.0f - firstLinePositionPercentage) + baseLineHeight * (1.0f / LINE_REDUCTION) * (firstLinePositionPercentage);

	float currentLineHeight = firstLineHeight;
	float actualRenderPos = startRenderPos;

	int actualLineIndex = firstLineIndex;

	bool reOrganizeLines = true;

	if (actualLineIndex == 0) nextTopLine = alphaLines - 1;
	else nextTopLine = actualLineIndex - 1;

	while(reOrganizeLines) 
	{
		float currentSegmentPrintedHeight = currentLineHeight * (1.0f - LINE_REDUCTION);
		alphaLinesArray[actualLineIndex].y = (int)actualRenderPos;
		renderLineValues[actualLineIndex] = actualRenderPos;
		alphaLinesArray[actualLineIndex].h = (int)currentSegmentPrintedHeight;
		//LOG("Actual Line - Index = %i, Y position = %f, H = %i", actualLineIndex, actualRenderPos, alphaLinesArray[actualLineIndex].h);
		currentLineHeight = currentLineHeight * LINE_REDUCTION;
		actualRenderPos -= currentLineHeight;

		actualLineIndex = (actualLineIndex + 1) % alphaLines;
		reOrganizeLines = !(actualLineIndex == firstLineIndex);
	}

	for (int i = 0; i < alphaLines; i++)
	{
		DrawQuad(alphaLinesArray[i], 0, 100, 0, 100);
	}

	float nextfirstSegmentPositionPercentage = fmod(firstLinePositionPercentage + 0.07f, 1.0f);
	if (nextfirstSegmentPositionPercentage < firstLinePositionPercentage) {
		firstLineIndex = (firstLineIndex + 1) % alphaLines;
	}

	firstLinePositionPercentage = nextfirstSegmentPositionPercentage;
}