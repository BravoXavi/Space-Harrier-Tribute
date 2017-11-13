#include "Font.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include <string>
#include <iostream>
#include <assert.h>

Font::Font(const char* pathToBMP, const char* traductor)
{
	fontPNG = App->textures->Load(pathToBMP);
	int textW, textH;

	SDL_QueryTexture(fontPNG, NULL, NULL, &textW, &textH);

	//Considering fixed known size of 16x16
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
	

	std::cout << "Font " << pathToBMP << " loaded succesfully!" << std::endl;
}

void Font::printText(const char* textToWrite, int x, int y) 
{	
	for (unsigned int i = 0; i < strlen(textToWrite); ++i) 
	{
		if (textToWrite[i] == ' ') simbolSize.y = 8;
		else simbolSize = simbolPosition[traductorMap[textToWrite[i]]];
		App->renderer->Blit(fontPNG, x + (i*8), y, &simbolSize);
	}
}