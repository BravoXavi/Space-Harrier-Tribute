#include "Globals.h"
#include "Application.h"
#include "MetalFlower.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

MetalFlower::MetalFlower()
{
}

MetalFlower::MetalFlower(const Enemy& aS)
{
	animationTimer = SDL_GetTicks();

	idle = aS.idle;
	transition = aS.transition;
	current_animation = aS.current_animation;
	position = aS.position;
	fxIndex = aS.fxIndex;
	uniDimensionalSpeed = aS.uniDimensionalSpeed;
	depthSpeed = aS.depthSpeed;
}

MetalFlower::~MetalFlower()
{
	delete rect;
	delete resizeRect;
}

//MetalFlower Update
void MetalFlower::Update()
{
	if (position.z <= MIN_Z || position.z > MAX_Z)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	Uint32 actualTicks = SDL_GetTicks();

	if (current_animation->GetCurrentFrame().x == 3) invulnerable = true;
	else invulnerable = false;

	if (actualTicks - animationTimer > 3000.0f)
	{
		animationTimer = actualTicks;

		if (current_animation->GetCurrentFrame().x == 78)
		{
			current_animation->reverse = true;
		}
		else if (current_animation->GetCurrentFrame().x == 3)
		{
			current_animation->reverse = false;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = current_animation->GetCurrentFrame().w * zModifier;
	float newHeight = current_animation->GetCurrentFrame().h * zModifier;
	float newX = position.x - newWidth / 2.0f;
	float newY = position.y - newHeight / 2.0f;

	//Move the enemy according to its preset movement
	selectMovementPatron(moveSet);

	newY += (-App->renderer->horizonY + (float)FLOOR_Y_MIN);

	if (collider != nullptr)
	{
		collider->SetPos((int)newX, (int)newY, (int)position.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}

	setResizeRect(newWidth, newHeight);
	setRect(App->enemies->graphics, newX, newY, position.z, &(current_animation->GetCurrentFrame()), resizeRect);
}

//MetalFlower movement patrons
void MetalFlower::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	switch (moveSelector)
	{
	case 1:
		break;
	default:
		break;
	}
}

//Return an instance of MetalFlower
Enemy* MetalFlower::createEnemyInstance(const Enemy& e) const
{
	Enemy* instance = new MetalFlower(e);
	return instance;
}