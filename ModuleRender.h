#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"
#include <map>
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

#define ALPHA_DISTANCE_MAX 20.0f*SCREEN_SIZE
#define ALPHA_DISTANCE_MIN 6.0f*SCREEN_SIZE
#define ALPHA_SIZE_MAX 10.3f*SCREEN_SIZE
#define ALPHA_SIZE_MIN 3.32f*SCREEN_SIZE

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
	void SetAlphaLineParametersPercentual(float percent);

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	int horizonY;
	float increasingExtraPixelsX = 0.0f;
	float playerSpeed = 0.0f;
	std::map<int, std::vector<BlitTarget>> depthBuffer;

private:
	float startDistanceBetweenAlphaLines;
	float distanceBetweenAlphaLines;

	float startSizeOfAlphaLines;
	float sizeOfAlphaLines;

	int iterationOfAlphaLine;
};

#endif // __MODULERENDER_H__