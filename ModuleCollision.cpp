#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"

using namespace std;

ModuleCollision::ModuleCollision()
{
	collisionMatrix[PLAYER][PLAYER] = false;
	collisionMatrix[PLAYER][P_LASER] = false;

	collisionMatrix[P_LASER][P_LASER] = false;
	collisionMatrix[P_LASER][E_LASER] = false;

	collisionMatrix[E_LASER][E_LASER] = false;
	collisionMatrix[E_LASER][D_OBSTACLE] = false;
	collisionMatrix[E_LASER][ND_OBSTACLE] = false;

	collisionMatrix[D_OBSTACLE][D_OBSTACLE] = false;
	collisionMatrix[D_OBSTACLE][ND_OBSTACLE] = false;

	collisionMatrix[ND_OBSTACLE][ND_OBSTACLE] = false;
}

// Destructor
ModuleCollision::~ModuleCollision()
{}

update_status ModuleCollision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end();)
	{
		if ((*it)->to_delete == true)
		{
			RELEASE(*it);
			it = colliders.erase(it);
		}
		else
			++it;
	}

	return UPDATE_CONTINUE;
}

update_status ModuleCollision::Update()
{
	// TODO 8: Check collisions between all colliders. 
	// After making it work, review that you are doing the minumum checks possible

	for (list<Collider*>::const_iterator it = colliders.cbegin(), end = colliders.cend(); it != end; it++)
	{
		for (list<Collider*>::const_iterator it2 = next(it, 1); it2 != end; it2++)
		{
			if (collisionMatrix[(*it)->colType][(*it2)->colType]) {
				(*it)->CheckCollision((*it2)->rect, (*it2)->depth);
			}
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(debug == true)
		DebugDraw();

	return UPDATE_CONTINUE;
}

void ModuleCollision::DebugDraw()
{
	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); ++it)
		App->renderer->DrawQuad((*it)->rect, 255, 0, 0, 80);
}

// Called before quitting
bool ModuleCollision::CleanUp()
{
	LOG("Freeing all colliders");

	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); ++it)
		RELEASE(*it);

	colliders.clear();

	return true;
}

Collider* ModuleCollision::AddCollider(const SDL_Rect& rect, collisionType colType, int zDepth)
{
	Collider* ret = new Collider(rect, colType, zDepth);

	colliders.push_back(ret);

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r, const int& depth) const
{
	bool xColl = true;
	bool yColl = true;
	bool zColl = false;

	if ( r.x > (this->rect.x + this->rect.w) || (r.x + r.w) < this->rect.x ) xColl = false;
	if ( r.y > (this->rect.y + this->rect.h) || (r.y + r.h) < this->rect.y ) yColl = false;

	if ( this->depth == depth || abs(this->depth - depth) <= 2) zColl = true;
	
	if (xColl && yColl && zColl) 
		LOG("COLLISION DETECTED!!!");

	return (xColl && yColl && zColl);
}
