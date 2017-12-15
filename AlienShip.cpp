#include "Globals.h"
#include "Application.h"
#include "AlienShip.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

AlienShip::AlienShip()
{
}

AlienShip::AlienShip(const Enemy& aS, const fPoint& pos)
{
	position = pos;
	enemyAnimation = aS.enemyAnimation;
	fxIndex = aS.fxIndex;
	uniDimensionalSpeed = aS.uniDimensionalSpeed;
	depthSpeed = aS.depthSpeed;
}

AlienShip::~AlienShip()
{
	delete rect;
	delete resizeRect;
}

//AlienShip Update
void AlienShip::Update()
{
	if (position.z <= MIN_Z || position.z > MAX_Z)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	float zModifier = 1.0f - (position.z / (float)MAX_Z);
	float newWidth = enemyAnimation.GetCurrentFrame().w * zModifier;
	float newHeight = enemyAnimation.GetCurrentFrame().h * zModifier;

	//Move the enemy according to its preset movement
	selectMovementPatron(moveSet);

	float newPosX = position.x - newWidth / 2.0f;
	float newPosY = position.y - newHeight / 2.0f;

	newPosY += (-App->renderer->horizonY + (float)FLOOR_Y_MIN);

	if (collider != nullptr)
	{
		collider->SetPos((int)newPosX, (int)newPosY, (int)position.z);
		collider->SetSize((int)newWidth, (int)newHeight);
	}

	setResizeRect(newWidth, newHeight);
	setRect(App->enemies->graphics, newPosX, newPosY, position.z, &(enemyAnimation.GetCurrentFrame()), resizeRect);
}

//AlienShip movement patrons
void AlienShip::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	switch (moveSelector)
	{
	case 1:
	{
		if (position.x < (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w * 2.0f) && position.y == (float)SCREEN_HEIGHT / 3.0f)
		{
			if (position.x > (float)SCREEN_WIDTH / 2.0f) position.x += deltaUniDimensionalSpeed*1.5f;
			else position.x += deltaUniDimensionalSpeed;
		}
		else
		{
			if (position.y == (float)SCREEN_HEIGHT / 3.0f) position.y = 2.0f * ((float)SCREEN_HEIGHT / 3.0f);
			position.x += -deltaUniDimensionalSpeed * 1.6f;
			position.z -= deltaDepthSpeed;
			position.y += 0.3f;
			if (position.x < -(float)enemyAnimation.GetCurrentFrame().w) position.y = (float)SCREEN_HEIGHT / 3.0f;
		}
		break;
	}
	case 2:
	{
		if (position.y > (float)SCREEN_HEIGHT / 3.0f && position.z == 17.0f)
		{
			position.y -= deltaUniDimensionalSpeed;
		}
		else
		{
			if (position.z == 17.0f) App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
			if (position.y < (3.0f * (float)SCREEN_HEIGHT / 4.0f))
			{
				position.y += deltaUniDimensionalSpeed;
				position.z -= deltaDepthSpeed / 3.0f;
			}
			else
			{
				position.z -= deltaDepthSpeed * 1.2f;
			}

		}
		break;
	}
	case 3:
	{
		float angleOffset = cos(oscillationAngle);

		if (position.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) oscillationSpeed *= -1.0f;		
		if (oscillationSpeed > 0.0f) angleOffset *= -1.0f;
		if (oscillationAngle >= 2.0f*M_PI || oscillationAngle <= -2.0f*M_PI)
		{
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
			oscillationAngle = 0.0f;
		}

		position.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
		oscillationAngle += oscillationSpeed;
		oscillationRadius -= 0.2f;
		position.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
		position.z += deltaDepthSpeed * 0.6;

		break;
	}
	case 4:
	{
		float angleOffset = cos(oscillationAngle);

		if (position.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) oscillationSpeed *= -1.0f;
		if (oscillationSpeed > 0.0f) angleOffset *= -1.0f;

		position.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
		oscillationAngle += oscillationSpeed;
		oscillationRadius -= 0.2f;
		
		if (oscillationAngle > (3.0f*M_PI) / 2.0f)
		{
			position.z -= deltaDepthSpeed * 0.8f;
			position.y += deltaUniDimensionalSpeed / 6.0f;
		}
		else
		{
			position.z += deltaDepthSpeed * 0.8f;
			position.y -= deltaUniDimensionalSpeed / 6.0f;
		}

		break;
	}
	case 5:
	{
		float angleOffset = cos(oscillationAngle);

		if (position.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) oscillationSpeed *= -1.0f;
		if (oscillationSpeed > 0.0f) angleOffset *= -1.0f;

		if (oscillationRadius != 0.0f)
		{
			position.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
			oscillationAngle += oscillationSpeed;
			oscillationRadius -= 0.2f;
			position.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
			position.z += deltaDepthSpeed * 0.6;
		}
		else
		{
			oscillationAngle = 0.0f;
			if (position.x < (float)SCREEN_WIDTH / 2.0f) position.x -= deltaUniDimensionalSpeed / 4.0f;
			else position.x += deltaUniDimensionalSpeed / 4.0f;
			position.z -= deltaDepthSpeed * 1.5f;
		}


		if (oscillationAngle >= 2.0f*M_PI || oscillationAngle <= -2.0f*M_PI)
		{
			oscillationRadius = 0.0f;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}

		break;
	}
	default:
		break;
	}	
}

//Return an instance of AlienShip
Enemy* AlienShip::createEnemyInstance(const Enemy& e, const fPoint& pos) const
{
	Enemy* instance = new AlienShip(e, pos);
	return instance;
}