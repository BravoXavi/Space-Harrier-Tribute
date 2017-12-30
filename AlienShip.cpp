#include "Globals.h"
#include "Application.h"
#include "AlienShip.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

AlienShip::AlienShip()
{
}

AlienShip::AlienShip(const Enemy& aS, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oAngle)
{
	worldPosition = pos;
	colType = cType;
	collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)worldPosition.z, App->enemies);
	moveSet = moveSelector;
	enemyAnimation = aS.enemyAnimation;
	uniDimensionalSpeed = aS.uniDimensionalSpeed;
	depthSpeed = aS.depthSpeed;

	oscillationAngle = oAngle;
	oscillationRadius = (float)SCREEN_WIDTH / 2.0f;
	oscillationSpeed = 0.03f;
}

AlienShip::~AlienShip()
{
	delete dataToBlit;
}

//AlienShip Update
void AlienShip::Update()
{
	if (worldPosition.z <= MIN_Z || worldPosition.z > MAX_Z)
	{
		collider->to_delete = true;
		to_delete = true;
	}

	float zModifier = 1.0f - (worldPosition.z / (float)MAX_Z);
	float newWidth = enemyAnimation.GetCurrentFrame().w * zModifier;
	float newHeight = enemyAnimation.GetCurrentFrame().h * zModifier;

	//Move the enemy according to its preset movement
	selectMovementPatron(moveSet);

	screenPosition.x = worldPosition.x - newWidth / 2.0f;
	screenPosition.y = worldPosition.y - newHeight / 2.0f;
	screenPosition.y += (-App->renderer->horizonY + (float)FLOOR_Y_MIN);
	screenPosition.z = worldPosition.z;

	if (collider != nullptr)
		collider->SetPos((int)screenPosition.x, (int)screenPosition.y, (int)worldPosition.z, (int)newWidth, (int)newHeight);

	setRect(App->enemies->graphics, screenPosition.x, screenPosition.y, screenPosition.z, newWidth, newHeight, &(enemyAnimation.GetCurrentFrame()));
}

//AlienShip movement patrons
const void AlienShip::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	switch (moveSelector)
	{
		case 1:
		{
			if (worldPosition.x < (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w * 2.0f) && worldPosition.y == (float)SCREEN_HEIGHT / 3.0f)
			{
				if (worldPosition.x > (float)SCREEN_WIDTH / 2.0f) 
					worldPosition.x += deltaUniDimensionalSpeed*1.5f;
				else 
					worldPosition.x += deltaUniDimensionalSpeed;
			}
			else
			{
				if (worldPosition.y == (float)SCREEN_HEIGHT / 3.0f) 
					worldPosition.y = 2.0f * ((float)SCREEN_HEIGHT / 3.0f);

				worldPosition.x += -deltaUniDimensionalSpeed * 1.6f;
				worldPosition.z -= deltaDepthSpeed;
				worldPosition.y += 0.3f;

				if (worldPosition.x < -(float)enemyAnimation.GetCurrentFrame().w) 
					worldPosition.y = (float)SCREEN_HEIGHT / 3.0f;
			}
			break;
		}
		case 2:
		{
			if (worldPosition.y > (float)SCREEN_HEIGHT / 3.0f && worldPosition.z == 17.0f)
			{
				worldPosition.y -= deltaUniDimensionalSpeed;
			}
			else
			{
				if (worldPosition.z == 17.0f) App->particles->AddParticle(App->particles->e_laser, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
				if (worldPosition.y < (3.0f * (float)SCREEN_HEIGHT / 4.0f))
				{
					worldPosition.y += deltaUniDimensionalSpeed;
					worldPosition.z -= deltaDepthSpeed / 3.0f;
				}
				else
				{
					worldPosition.z -= deltaDepthSpeed * 1.2f;
				}
			}
			break;
		}
		case 3:
		{
			float angleOffset = cos(oscillationAngle);

			if (worldPosition.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) 
				oscillationSpeed *= -1.0f;

			if (oscillationSpeed > 0.0f) 
				angleOffset *= -1.0f;

			if (oscillationAngle >= 2.0f*M_PI || oscillationAngle <= -2.0f*M_PI)
			{
				App->particles->AddParticle(App->particles->e_laser, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
				oscillationAngle = 0.0f;
			}

			worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
			oscillationAngle += oscillationSpeed;
			oscillationRadius -= 0.2f;
			worldPosition.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
			worldPosition.z += deltaDepthSpeed * 0.6f;
			break;
		}
		case 4:
		{
			float angleOffset = cos(oscillationAngle);

			if (worldPosition.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) 
				oscillationSpeed *= -1.0f;

			if (oscillationSpeed > 0.0f) 
				angleOffset *= -1.0f;

			worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
			oscillationAngle += oscillationSpeed;
			oscillationRadius -= 0.2f;
		
			if (oscillationAngle > (3.0f*M_PI) / 2.0f)
			{
				worldPosition.z -= deltaDepthSpeed * 0.8f;
				worldPosition.y += deltaUniDimensionalSpeed / 6.0f;
			}
			else
			{
				worldPosition.z += deltaDepthSpeed * 0.8f;
				worldPosition.y -= deltaUniDimensionalSpeed / 6.0f;
			}
			break;
		}
		case 5:
		{
			float angleOffset = cos(oscillationAngle);

			if (worldPosition.x == (float)(SCREEN_WIDTH + enemyAnimation.GetCurrentFrame().w)) 
				oscillationSpeed *= -1.0f;

			if (oscillationSpeed > 0.0f) 
				angleOffset *= -1.0f;

			if (oscillationRadius != 0.0f)
			{
				worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + angleOffset * oscillationRadius;
				oscillationAngle += oscillationSpeed;
				oscillationRadius -= 0.2f;
				worldPosition.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
				worldPosition.z += deltaDepthSpeed * 0.6f;
			}
			else
			{
				oscillationAngle = 0.0f;
				if (worldPosition.x < (float)SCREEN_WIDTH / 2.0f) worldPosition.x -= deltaUniDimensionalSpeed / 4.0f;
				else worldPosition.x += deltaUniDimensionalSpeed / 4.0f;
				worldPosition.z -= deltaDepthSpeed * 1.5f;
			}

			if (oscillationAngle >= 2.0f*M_PI || oscillationAngle <= -2.0f*M_PI)
			{
				oscillationRadius = 0.0f;
				App->particles->AddParticle(App->particles->e_laser, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
			}
			break;
		}
	}	
}

//Return an instance of AlienShip
Enemy* AlienShip::createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oscillationAngle) const
{
	Enemy* instance = new AlienShip(e, pos, cType, moveSelector, oscillationAngle);
	return instance;
}