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
	void CleanUp();

public:
	Font* blueFont;

};

#endif // !CLASS_FONTMANAGER
