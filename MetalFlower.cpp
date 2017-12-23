#include "Globals.h"
#include <math.h>
#include "Application.h"
#include "MetalFlower.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

MetalFlower::MetalFlower()
{}

MetalFlower::MetalFlower(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oAngle)
{
	animationTimer = SDL_GetTicks();
	worldPosition = pos;
	collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)worldPosition.z, App->enemies);	
	moveSet = moveSelector;
	colType = cType;
	enemyAnimation = e.enemyAnimation;
	fxIndex = e.fxIndex;
	uniDimensionalSpeed = e.uniDimensionalSpeed;
	depthSpeed = e.depthSpeed;

	oscillationAngle = oAngle;
	oscillationSpeed = 0.015f;
	oscillationRadius = 40.0f;
}

MetalFlower::~MetalFlower()
{
	delete dataToBlit;
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

	//MODIFY TO USE CURRENT_FRAME
	if (enemyAnimation.GetCurrentFrame().x == 7) collider->colType = ND_ENEMY;
	else collider->colType = ENEMY;

	if (moveSet == 1)
	{
		if (actualTicks - animationTimer > 2000.0f)
		{
			animationTimer = actualTicks;
			if (enemyAnimation.GetCurrentFrame().x == 82)
			{
				collider->colType = ND_ENEMY;
				enemyAnimation.reverseAnimation = true;
			}
			else if (enemyAnimation.GetCurrentFrame().x == 7)
			{
				collider->colType = ENEMY;
				enemyAnimation.reverseAnimation = false;
			}

			if (!enemyAnimation.reverseAnimation) App->particles->AddParticle(App->particles->e_laser, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
		}
	}
	else
	{
		enemyAnimation.reverseAnimation = true;
		collider->colType = ND_ENEMY;
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
		collider->SetPos((int)screenPosition.x, (int)screenPosition.y, (int)worldPosition.z, (int)newWidth, (int)newHeight);
	}

	setRect(App->enemies->graphics, screenPosition.x, screenPosition.y, screenPosition.z, newWidth, newHeight, &(enemyAnimation.GetCurrentFrame()));
}

//MetalFlower movement patrons
void MetalFlower::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + cos(oscillationAngle) * oscillationRadius;
	worldPosition.y = 1.7f * ((float)SCREEN_HEIGHT / 3.0f) + sin(oscillationAngle) * oscillationRadius;
	oscillationAngle += oscillationSpeed;

	if (oscillationAngle >= 2.0f*M_PI) oscillationAngle = 0.0f;

	switch (moveSelector)
	{
	case 1:	
		if (worldPosition.z >= 10.0f)
		{
			if (collider->colType == ND_ENEMY)
			{
				oscillationRadius += 0.1f;
				worldPosition.z -= deltaDepthSpeed;
			}
		}
		else
		{
			moveSet++;
		}
		break;

	case 2:
		oscillationSpeed = 0.05f;
		if(oscillationRadius > 0.0f) oscillationRadius -= 0.8f;
		worldPosition.z += deltaDepthSpeed * 4.0f;

		break;

	default:
		break;
	}
}

//Return an instance of MetalFlower
Enemy* MetalFlower::createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oscillationAngle) const
{
	Enemy* instance = new MetalFlower(e, pos, cType, moveSelector, oscillationAngle);
	return instance;
}