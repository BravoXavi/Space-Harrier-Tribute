#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"
#include "Font.h"
#include <map>
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

struct BlitTarget
{	
	float x;
	float y;
	float z;
	float newWidth;
	float newHeight;
	SDL_Rect* section;
	SDL_Texture* texture;

	BlitTarget(SDL_Texture* texture, float x, float y, float z, float newWidth, float newHeight, SDL_Rect* section) :
		texture(texture),
		x(x),
		y(y),
		z(z),
		newWidth(newWidth),
		newHeight(newHeight),
		section(section)	
	{}
};

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void DrawPauseScreen() const;
	void AlphaVerticalLinesMove();

	bool Blit(SDL_Texture* texture, float x, float y, SDL_Rect* section, const float& newWidth = -1.0f, const float& newHeight = -1.0f) const;
	bool FloorBlit(SDL_Texture* texture, SDL_Rect* section);
	void BackgroundBlit(SDL_Texture* texture, float speed, int backgroundPlane);	
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera = true) const;

public:
	SDL_Renderer* renderer = nullptr;	

	static const int alphaLines = 11;
	static const float LINE_REDUCTION;
	
	int firstLineIndex = 0;
	int nextTopLine = 0;

	float horizonY;
	float increasingExtraPixelsX = 0.0f;
	float playerSpeed = 0.0f;
	float lineDivisor = 0.0f;
	float firstLinePositionPercentage = 0.0f;
	float backgroundOffset_B = 0.0f;
	float backgroundOffset_BF = 0.0f;
	float renderLineValues[alphaLines];

	std::map<int, std::vector<BlitTarget>> depthBuffer;
	SDL_Rect alphaLinesArray[alphaLines];

};

#endif // __MODULERENDER_H__