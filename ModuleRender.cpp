#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "FontManager.h"
#include "SDL/include/SDL.h"

const float ModuleRender::LINE_REDUCTION = 0.60f;

ModuleRender::ModuleRender()
{
	horizonY = (float)FLOOR_Y_MIN;
	actualHorizonY = (float)FLOOR_Y_MIN;

	for (int i = 0; i < alphaLines; i++)
	{
		renderLineValues[i] = 0.0f;
		alphaLinesArray[i].x = 0;
		alphaLinesArray[i].w = SCREEN_WIDTH * SCREEN_SIZE;
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
				Blit(itt->texture, itt->x, itt->y, itt->section, itt->newWidth, itt->newHeight);
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
bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
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
bool ModuleRender::Blit(SDL_Texture* texture, float x, float y, SDL_Rect* section, const float& newWidth, const float& newHeight)
{
	bool ret = true;

	SDL_Rect rect;

	rect.x = (int)(x * SCREEN_SIZE);
	rect.y = (int)(y * SCREEN_SIZE);

	if (newWidth >= 0.0f && newHeight >= 0.0f)
	{
		rect.w = (int)newWidth;
		rect.h = (int)newHeight;
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

//Blit background layer to screen
void ModuleRender::BackgroundBlit(SDL_Texture* background, float speed, int backgroundPlane)
{
	SDL_Rect rect;
	SDL_Rect screenCutLeft;
	float offset = 0.0f;
	int textW;
	int textH;

	rect.x = 0;
	rect.y = 0;
	SDL_QueryTexture(background, NULL, NULL, &textW, &textH);

	if (backgroundPlane == 1)
	{
		backgroundOffset_B -= playerSpeed * speed;
		offset = backgroundOffset_B;
		rect.y -= ((int)horizonY * SCREEN_SIZE) - 3;
		rect.h = SCREEN_HEIGHT * SCREEN_SIZE;
	}
	else
	{
		backgroundOffset_BF -= playerSpeed * speed;
		offset = backgroundOffset_BF;
		rect.y = ((SCREEN_HEIGHT - (int)horizonY) - textH) * SCREEN_SIZE + 3;
		rect.h = textH * SCREEN_SIZE;
	}

	rect.x = (int)offset;	
	rect.w = textW * SCREEN_SIZE;

	screenCutLeft.x = - (textW * SCREEN_SIZE) + (int)offset;
	screenCutLeft.y = rect.y;
	screenCutLeft.w = rect.w;
	screenCutLeft.h = rect.h;

	if (offset > 0.0f && offset < (float)(SCREEN_WIDTH * SCREEN_SIZE))
	{
		SDL_RenderCopy(renderer, background, nullptr, &screenCutLeft);
		SDL_RenderCopy(renderer, background, nullptr, &rect);
	}
	else if (offset > (float)(SCREEN_WIDTH * SCREEN_SIZE))
	{
		SDL_RenderCopy(renderer, background, nullptr, &screenCutLeft);
	}
	else if (offset <= (float)((-textW + SCREEN_WIDTH) * SCREEN_SIZE))
	{
		screenCutLeft.x = (textW * SCREEN_SIZE) + rect.x;
		screenCutLeft.y = rect.y;
		screenCutLeft.w = rect.w;
		screenCutLeft.h = rect.h;
		SDL_RenderCopy(renderer, background, nullptr, &screenCutLeft);
		SDL_RenderCopy(renderer, background, nullptr, &rect);
	}
	else
	{
		SDL_RenderCopy(renderer, background, nullptr, &rect);
	}
	if (offset > (float)(textW * SCREEN_SIZE) || offset <= (float)(-textW * SCREEN_SIZE)) 
	{
		if(backgroundPlane == 1) backgroundOffset_B = 0.0f;
		else backgroundOffset_BF = 0.0f;
	}
}

// Blit floor to screen ( (0,0) will be the down-mid of the screen )
bool ModuleRender::FloorBlit(SDL_Texture* texture, SDL_Rect* section)
{
	bool ret = true;
	SDL_Rect rect;
	int textW, textH;

	if (!App->player->gotHit)
	{
		if (horizonY > actualHorizonY - 0.3f) horizonY -= 0.3f;
		else if (horizonY < actualHorizonY + 0.3f) horizonY += 0.3f;
	}

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

//Function moving the horizontal descending lines - Avoid modifying it unless absolutely necessary!! ( Messy but fast :) )
void ModuleRender::AlphaVerticalLinesMove()
{	
	if (!App->player->gotHit)
	{
		float baseLineHeight = (float)(horizonY * SCREEN_SIZE) / lineDivisor;

		float startRenderPos = (float)SCREEN_HEIGHT * (float)SCREEN_SIZE - baseLineHeight * (1.0f - firstLinePositionPercentage);
		float firstLineHeight = baseLineHeight * (1.0f - firstLinePositionPercentage) + baseLineHeight * (1.0f / LINE_REDUCTION) * (firstLinePositionPercentage);

		float currentLineHeight = firstLineHeight;
		float actualRenderPos = startRenderPos;

		int actualLineIndex = firstLineIndex;

		if (actualLineIndex == 0) nextTopLine = alphaLines - 1;
		else nextTopLine = actualLineIndex - 1;

		bool reOrganizeLines = true;
		while (reOrganizeLines)
		{
			float currentSegmentPrintedHeight = currentLineHeight * (1.0f - LINE_REDUCTION);

			alphaLinesArray[actualLineIndex].y = (int)actualRenderPos;
			renderLineValues[actualLineIndex] = actualRenderPos;
			alphaLinesArray[actualLineIndex].h = (int)currentSegmentPrintedHeight;

			currentLineHeight = currentLineHeight * LINE_REDUCTION;
			actualRenderPos -= currentLineHeight;
			actualLineIndex = (actualLineIndex + 1) % alphaLines;
			reOrganizeLines = !(actualLineIndex == firstLineIndex);
		}
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

//Draw pause screen when pause mode is active
void ModuleRender::DrawPauseScreen() const
{
	SDL_Rect screen;
	screen.x = 0;
	screen.y = 0;
	screen.w = SCREEN_WIDTH * SCREEN_SIZE;
	screen.h = SCREEN_HEIGHT * SCREEN_SIZE;

	DrawQuad(screen, 0, 0, 0, 150);
	App->fontManager->blueFont->printText("PAUSED", (SCREEN_WIDTH / 2) - 24, SCREEN_HEIGHT / 2);
}