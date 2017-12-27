#include "Globals.h"
#include <math.h>
#include "Application.h"
#include "DragonPart.h"
#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleEnemy.h"
#include "ModulePlayer.h"

DragonPart::DragonPart()
{}

DragonPart::DragonPart(const Enemy& e, const fPoint& pos, const collisionType& cType, const int& moveSelector, const float& oAngle)
{
	worldPosition = pos;
	colType = cType;
	collider = App->collision->AddCollider({ 0, 0, 0, 0 }, colType, (int)worldPosition.z, App->enemies);
	moveSet = moveSelector;
	enemyAnimation = e.enemyAnimation;
	fxIndex = e.fxIndex;
	uniDimensionalSpeed = e.uniDimensionalSpeed;
	depthSpeed = e.depthSpeed;

	oscillationAngle = oAngle;
	oscillationX = oAngle;
	oscillationY = oAngle;
	oscillationSpeed = 0.02f;

	lifePoints = 8;
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
		App->particles->AddParticle(App->particles->explosion, screenPosition.x, screenPosition.y, screenPosition.z, EXPLOSION);
		App->player->playerScore += 2000.0f;
	}

	if (superiorBodyPart != nullptr)
	{
		lifePoints = superiorBodyPart->lifePoints;
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
		forward = true;
		enemyAnimation.current_frame = 0;
		worldPosition.z = 20.0f;
		depthSpeed *= -1;
	}
	else if (worldPosition.z < 3.0f && forward)
	{
		forward = false;
		enemyAnimation.current_frame = 1;
		worldPosition.z = 5.0f;
		depthSpeed *= -1;
	}

	worldPosition.x = ((float)SCREEN_WIDTH / 2.0f) + cos(oscillationX) * (float)SCREEN_WIDTH / 2;
	worldPosition.y = ((float)SCREEN_HEIGHT / 2.0f) + cos(oscillationY) * 50.0f;

	oscillationX += oscillationSpeed/2;
	oscillationY += oscillationSpeed;

	if (oscillationAngle >= 2.0f*M_PI) oscillationAngle = 0.0f;

	Uint32 shotCheck = SDL_GetTicks();

	if (superiorBodyPart == nullptr && worldPosition.z > 10.0f && shotCheck - fireBallTimer > 3000.0f)
	{
		fireBallTimer = shotCheck;
		App->particles->AddParticle(App->particles->fireBall, worldPosition.x, worldPosition.y, worldPosition.z, E_LASER);
		App->particles->AddParticle(App->particles->fireBall, worldPosition.x, worldPosition.y + 15, worldPosition.z, E_LASER);
		App->particles->AddParticle(App->particles->fireBall, worldPosition.x + 15, worldPosition.y, worldPosition.z, E_LASER);
	}
}

//Return an instance of DragonHead
Enemy* DragonPart::createEnemyInstance(const Enemy& e, const fPoint& pos, const collisionType& colType, const int& moveSelector, const float& oscillationAngle) const
{
	Enemy* instance = new DragonPart(e, pos, colType, moveSelector, oscillationAngle);
	return instance;
}