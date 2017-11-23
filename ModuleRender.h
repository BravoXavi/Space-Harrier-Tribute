#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"
#include <map>
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

struct BlitTarget
{
	SDL_Texture* texture;
	int x;
	int y;
	SDL_Rect* section;
	SDL_Rect* resize;
	int depth;

	BlitTarget(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, int depth) :
		texture(texture),
		x(x),
		y(y),
		section(section),
		resize(resize),
		depth(depth)
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

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, SDL_Rect* resize, float speed = 1.0f);

	bool FloorBlit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed = 1.0f);
	void AlphaVerticalLinesMove();

	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera = true);

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;	

	static const float LINE_REDUCTION;
	static const int alphaLines = 11;
	
	float horizonY;
	int firstLineIndex;
	int nextTopLine;
	float increasingExtraPixelsX = 0.0f;
	float playerSpeed = 0.0f;
	float lineDivisor = 0.0f;
	float firstLinePositionPercentage;

	std::map<int, std::vector<BlitTarget>> depthBuffer;
	SDL_Rect alphaLinesArray[alphaLines];
	float renderLineValues[alphaLines];
};

#endif // __MODULERENDER_H__