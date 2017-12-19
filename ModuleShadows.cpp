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

void ModuleShadows::DrawShadow(const float&x, const float& y, const float& z, const float& width, const float& height)
{
	BlitTarget* temp = new BlitTarget(graphics, x, y, z, width, height, &shadowPosition);
	App->renderer->depthBuffer[(int)temp->z].push_back(*temp);

	delete temp;
}