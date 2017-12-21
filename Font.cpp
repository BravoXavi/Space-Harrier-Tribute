#include "Font.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include <string>
#include <iostream>
#include <assert.h>

Font::Font(const char* pathToImage, const char* traductor)
{
	fontPNG = App->textures->Load(pathToImage);
	int textW, textH;

	SDL_QueryTexture(fontPNG, NULL, NULL, &textW, &textH);

	//Considering fixed known size game fonts
	characterWidth = 8;
	characterHeight = textH;

	int charCounter = 0;

	for (int u = 0; u < 45; u++)
	{
		simbolPosition[charCounter].x = (u * characterWidth);
		simbolPosition[charCounter].y = 0;
		simbolPosition[charCounter].w = characterWidth;
		simbolPosition[charCounter].h = characterHeight;

		traductorMap[traductor[charCounter]] = charCounter;
		++charCounter;
	}
}

Font::~Font()
{
	App->textures->Unload(fontPNG);
}

void Font::printText(const char* textToWrite, const float& x, const float& y, float resize)
{	
	for (unsigned int i = 0; i < strlen(textToWrite); ++i) 
	{
		if (textToWrite[i] == ' ') simbolSize.y = 8;
		else simbolSize = simbolPosition[traductorMap[textToWrite[i]]];

		if (resize != 1.0f)
		{
			int resizedWidth = simbolSize.w * resize;
			int resizedHeight = simbolSize.h * resize;
			App->renderer->Blit(fontPNG, x + ((float)i * resizedWidth), y, &simbolSize, (float)resizedWidth, (float)resizedHeight);
		}
		else
		{
			App->renderer->Blit(fontPNG, x + ((float)i * simbolSize.w), y, &simbolSize);
		}
	}
}