#include "Globals.h"
#include "Application.h"
#include "ModuleShadows.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "SDL/include/SDL.h"

using namespace std;

ModuleShadows::ModuleShadows(bool start_enabled) : Module(start_enabled)
{}

// Destructor
ModuleShadows::~ModuleShadows()
{}

bool ModuleShadows::Start()
{
	graphics = App->textures->Load("assets/Character_full.png");
	shadowPosition = { 197, 76, 43, 20 };

	return true;
}

update_status ModuleShadows::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleShadows::CleanUp()
{
	return true;
}

void ModuleShadows::DrawShadow(const float&x, const float& y, const float& z, const float& ownerWidth)
{
	float scaleValue = 1.0f - (z / (float)MAX_Z);
	if (scaleValue < 0.0f) scaleValue = 0.0f;

	float scaledWidth = shadowPosition.w * scaleValue;
	float scaledHeight = shadowPosition.h * scaleValue;

	float xOwnerOffset = (x + (ownerWidth / 2.0f));
	float scaledX = xOwnerOffset - (scaledWidth / 2.0f);

	float minPosition = (float)SCREEN_HEIGHT - App->renderer->horizonY;
	float scaledY = (scaleValue * ((float)SCREEN_HEIGHT - minPosition)) + minPosition;

	BlitTarget* temp = new BlitTarget(graphics, scaledX, scaledY - scaledHeight/2.0f, z, scaledWidth, scaledHeight, &shadowPosition);
	App->renderer->depthBuffer[(int)temp->z].push_back(*temp);

	delete temp;
}