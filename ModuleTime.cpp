#include "Globals.h"
#include "Application.h"
#include "ModuleTime.h"
#include "SDL/include/SDL.h"

using namespace std;

ModuleTime::ModuleTime(bool start_enabled) : Module(start_enabled)
{}

// Destructor
ModuleTime::~ModuleTime()
{}

bool ModuleTime::Init()
{
	deltaTime = 0.0f;
	lastDeltaTime = 0.0f;

	return true;
}

update_status ModuleTime::PreUpdate()
{
	Uint32 tick = SDL_GetTicks();
	deltaTime = (tick - lastDeltaTime) / 1000.0f;
	lastDeltaTime = tick;

	return UPDATE_CONTINUE;
}

float ModuleTime::getDeltaTime() const
{
	return deltaTime;
}

// Called before quitting
bool ModuleTime::CleanUp()
{
	return true;
}