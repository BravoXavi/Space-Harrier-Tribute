#ifndef CLASS_FONT
#define CLASS_FONT

#include "SDL/include/SDL.h"
#include <stdio.h>
#include <string>
#include <map>

class Font
{
public:
	Font(const char* fontBMP, const char* traductor);
	~Font();
		
	void printText(const char* textToWrite, int x, int y);

public:
	int characterHeight, characterWidth;

	SDL_Rect simbolPosition[45];
	SDL_Rect simbolSize;

private:
	SDL_Surface* fontBMP = nullptr;
	SDL_Texture* fontPNG = nullptr;
	std::map<char, int> traductorMap;

};

#endif // !CLASS_FONT
