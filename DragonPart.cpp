#include "Globals.h"
#include <math.h>
#include "Application.h"
#include "DragonPart.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"

DragonPart::DragonPart()
{}

DragonPart::DragonPart(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector)
{
	worldPosition = pos;
	colType = cType;
	collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)worldPosition.z, App->enemies);
	moveSet = moveSelector;
	enemyAnimation = e.enemyAnimation;
	fxIndex = e.fxIndex;
	uniDimensionalSpeed = e.uniDimensionalSpeed;
	depthSpeed = e.depthSpeed;
}

DragonPart::~DragonPart()
{
	delete dataToBlit;
}

//DragonHead Update
void DragonPart::Update()
{
	if (superiorBodyPart != nullptr && superiorBodyPart->to_delete)
	{
		collider->to_delete = true;
		to_delete = true;	
	}

	float zModifier = 1.0f - (worldPosition.z / (float)MAX_Z);
	if (zModifier < 0.0f) zModifier = 0.0f;
	float newWidth = enemyAnimation.GetCurrentFrame().w * zModifier;
	float newHeight = enemyAnimation.GetCurrentFrame().h * zModifier;

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

//DragonHead movement patrons
void DragonPart::selectMovementPatron(const int& moveSelector)
{
	float deltaUniDimensionalSpeed = uniDimensionalSpeed * App->time->getDeltaTime();
	float deltaDepthSpeed = depthSpeed * App->time->getDeltaTime();
	worldPosition.z += deltaDepthSpeed;

	if (worldPosition.z > 20.0f && !forward)
	{
		enemyAnimation.current_frame = 0;
		forward = true;
		worldPosition.z = 20.0f;
		depthSpeed *= -1;
	}
	else if (worldPosition.z < 5.0f && forward)
	{
		forward = false;
		enemyAnimation.current_frame = 1;
		worldPosition.z = 5.0f;
		depthSpeed *= -1;
	}

	switch (moveSelector)
	{
		case 1:	
		break;
	}
}

//Return an instance of DragonHead
Enemy* DragonPart::createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& colType, const int& moveSelector) const
{
	Enemy* instance = new DragonPart(e, pos, colType, moveSelector);
	return instance;
}