#include "FontManager.h"

FontManager::FontManager()
{}

FontManager::~FontManager()
{}

void FontManager::Init()
{
	blueFont = new Font("assets/blueFont.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<>,?¿!¡ ");
}

void FontManager::CleanUp()
{
	delete blueFont;
}