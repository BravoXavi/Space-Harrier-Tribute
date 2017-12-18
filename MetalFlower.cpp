#include "Globals.h"
#include <math.h>
#include "Application.h"
#include "MetalFlower.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

MetalFlower::MetalFlower()
{}

MetalFlower::MetalFlower(const Enemy& e, const fPoint& pos)
{
	animationTimer = SDL_GetTicks();

	worldPosition = pos;
	rotationAngle = pos.x;

	enemyAnimation = e.enemyAnimation;
	fxIndex = e.fxIndex;
	uniDimensionalSpeed = e.uniDimensionalSpeed;
	depthSpeed = e.depthSpeed;
}

MetalFlower::~MetalFlower()
{
	delete rect;
	delete resizeRect;
}

//MetalFlower Update
void MetalFlower::Update()
{
	if (worldPosition.z <= MIN_Z || worldPosition.z > MAX_Z)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	Uint32 actualTicks = SDL_GetTicks();

	if (enemyAnimation.GetCurrentFrame().x == 3) invulnerable = true;
	else invulnerable = false;

	if (moveSet == 1)
	{
		if (actualTicks - animationTimer > 1000.0f)
		{
			animationTimer = actualTicks;
			if (enemyAnimation.GetCurrentFrame().x == 78)
			{
				enemyAnimation.reverseAnimation = true;
			}
			else if (enemyAnimation.GetCurrentFrame().x == 3)
			{
				enemyAnimation.reverseAnimation = false;
			}

			if (!enemyAnimation.reverseAnimation) App->particles->AddParticle(App->particles->e_laser, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
		}
	}
	else
	{
		enemyAnimation.reverseAnimation = true;
		invulnerable = true;
	}

	float zModifier = 1.0f - (worldPosition.z / (float)MAX_Z);
	float newWidth = (float)enemyAnimation.GetCurrentFrame().w * zModifier;
	float newHeight = (float)enemyAnimation.GetCurrentFrame().h * zModifier;
	
	//Move the enemy according to its preset movement
	selectMovementPatron(moveSet);

	screenPosition.x = worldPosition.x - newWidth / 2.0f;
	screenPosition.y = worldPosition.y - newHeight / 2.0f;
	screenPosition.y += (-App->renderer->horizonY + (float)FLOOR_Y_MIN);
	screenPosition.z = worldPosition.z;

	if (collider != nullptr)
	{
		collider->SetPos((int)screenPosition.x, (int)screenPosition.y, (int)worldPosition.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}

	setResizeRect(newWidth, newHeight);
	setRect(App->enemies->graphics, screenPosition.x, screenPosition.y, screenPosition.z, &(enemyAnimation.GetCurrentFrame()), resizeRect);
}

//MetalFlower movement patrons
void MetalFlower::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + cos(rotationAngle) * spinRadius;
	worldPosition.y = 1.7f * ((float)SCREEN_HEIGHT / 3.0f) + sin(rotationAngle) * spinRadius;
	rotationAngle += spinSpeed;

	if (rotationAngle >= 2.0f*M_PI) rotationAngle = 0.0f;

	switch (moveSelector)
	{
	case 1:	
		if (worldPosition.z >= 10.0f)
		{
			if (invulnerable)
			{
				spinRadius += 0.1;
				worldPosition.z -= deltaDepthSpeed;
			}
		}
		else
		{
			moveSet++;
		}
		break;

	case 2:
		spinSpeed = 0.05f;
		if(spinRadius > 0.0f) spinRadius -= 0.8f;
		worldPosition.z += deltaDepthSpeed * 4.0f;

		break;

	default:
		break;
	}
}

//Return an instance of MetalFlower
Enemy* MetalFlower::createEnemyInstance(const Enemy& e, const fPoint& pos) const
{
	Enemy* instance = new MetalFlower(e, pos);
	return instance;
}