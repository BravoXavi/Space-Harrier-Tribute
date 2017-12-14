#include "Globals.h"
#include "Application.h"
#include "AlienShip.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

AlienShip::AlienShip()
{
}

AlienShip::AlienShip(const Enemy& aS)// : anim(aS.anim), position(aS.position), fxIndex(aS.fxIndex), depthSpeed(aS.depthSpeed)
{
	idle = aS.idle;
	transition = aS.transition;
	current_animation = aS.current_animation;
	position = aS.position;
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

//AlienShip movement patrons
void AlienShip::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();

	switch (moveSelector)
	{
	case 1:
		if (position.x < (float)(SCREEN_WIDTH + current_animation->GetCurrentFrame().w * 2.0f) && position.y == (float)SCREEN_HEIGHT / 3.0f)
		{
			if (position.x >(float)SCREEN_WIDTH / 2.0f) position.x += deltaUniDimensionalSpeed*1.5f;
			else position.x += deltaUniDimensionalSpeed;
		}
		else
		{
			if (position.y == (float)SCREEN_HEIGHT / 3.0f) position.y = 2.0f * ((float)SCREEN_HEIGHT / 3.0f);
			position.x += -deltaUniDimensionalSpeed * 1.6f;
			position.z -= deltaDepthSpeed;
			position.y += 0.3f;
			if (position.x < -(float)current_animation->GetCurrentFrame().w) position.y = (float)SCREEN_HEIGHT / 3.0f;
		}
 		break;

	case 2:
		if (position.y >(float)SCREEN_HEIGHT / 3.0f && position.z == 17.0f)
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

	case 3:
		if (position.x >= (float)(SCREEN_WIDTH + current_animation->GetCurrentFrame().w))
		{
			uniDimensionalSpeed *= -1;
			deltaUniDimensionalSpeed *= -1;
		}

		if (position.x < ((float)SCREEN_WIDTH / 5.0f) && deltaUniDimensionalSpeed < 0.0f)
		{
			uniDimensionalSpeed *= -1.0f;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}
		else if (position.x >(4.0f * ((float)SCREEN_WIDTH / 5.0f)) && deltaUniDimensionalSpeed > 0.0f)
		{
			uniDimensionalSpeed *= -1.0f;
			App->particles->AddParticle(App->particles->e_laser, position.x, position.y, position.z, E_LASER);
		}

		position.x += deltaUniDimensionalSpeed;
		position.y -= abs(deltaUniDimensionalSpeed) / 6.0f;
		position.z += deltaDepthSpeed * 0.8f;
		break;

	default:
		break;
	}	
}

//Return an instance of AlienShip
Enemy* AlienShip::createEnemyInstance(const Enemy& e) const
{
	Enemy* instance = new AlienShip(e);
	return instance;
}