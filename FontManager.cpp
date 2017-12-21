#include "FontManager.h"

FontManager::FontManager()
{}

FontManager::~FontManager()
{}

void FontManager::Init()
{
	blueFont = new Font("assets/blueFont.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<>,?¿!¡ ");
	redFont = new Font("assets/redFont.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<>,?¿!¡ ");
	greenFont = new Font("assets/greenFont.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.<>,?¿!¡ ");
}

void FontManager::CleanUp()
{
	delete blueFont;
	delete redFont;
	delete greenFont;
}