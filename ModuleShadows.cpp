#include "Globals.h"
#include "Application.h"
#include "ModuleShadows.h"
#include "SDL/include/SDL.h"

using namespace std;

ModuleShadows::ModuleShadows(bool start_enabled) : Module(start_enabled)
{}

// Destructor
ModuleShadows::~ModuleShadows()
{}

bool ModuleShadows::Init()
{
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