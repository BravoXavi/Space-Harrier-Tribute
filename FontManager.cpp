#include "FontManager.h"

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

void FontManager::Start()
{
	Font* blueFont = new Font("assets/blueFont.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<>,?¿!¡ ");
	fontMap["blue"] = blueFont;
}

Font* FontManager::addReference(const char* fontName)
{
	return fontMap[fontName];
}

void FontManager::removeReferences() 
{
	fontMap.clear();
	//delete fontMap["black"];
	//delete fontMap["blue"];
}

void FontManager::printWithFont(Font* font, int x, int y, const char* textToWrite)
{
	font->printText(textToWrite, x, y);
}