#ifndef CLASS_FONTMANAGER
#define CLASS_FONTMANAGER

#include <map>
#include "Font.h"

class FontManager 
{
public:
	FontManager();
	~FontManager();

	void Init();
	void removeReferences();
	void printWithFont(Font* font, int x, int y, const char* textToWrite);

	Font* addReference(const char* fontName);

private:
	std::map<const char*, Font*> fontMap;

	//guardar referencia map Font count //
};

#endif // !CLASS_FONTMANAGER
