#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModuleFadeToBlack.h"
#include "ModulePlayer.h"
#include "ModuleEnemy.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModulePlayer::ModulePlayer(bool active) : Module(active)
{

	run.frames.push_back({ 4, 4, 20, 47 });
	run.frames.push_back({ 25, 4, 20, 47 });
	run.frames.push_back({ 49, 2, 25, 49 });
	run.frames.push_back({ 75, 3, 21, 47 });
	run.speed = 0.15f;

	middle.frames.push_back({ 108,2,26,49 });

	left1.frames.push_back({ 142,2,22,50 });
	left2.frames.push_back({ 170,3,20,48 });

	right2.frames.push_back({ 197,3,20,48 });
	right1.frames.push_back({ 221,2,22,50 });

	current_animation = &run;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	graphics = App->textures->Load("assets/character.png");

	destroyed = false;
	position.x = (SCREEN_WIDTH/2) - (current_animation->GetCurrentFrame().w/2);
	position.y = SCREEN_HEIGHT - current_animation->GetCurrentFrame().h;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(graphics);

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int speed = 3;

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if(position.x > 0) position.x -= speed;
		if(current_animation == &run) checkHorizontalAnimation(true);
		else checkHorizontalAnimation();
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (position.x + current_animation->GetCurrentFrame().w < SCREEN_WIDTH) position.x += speed;
		if (current_animation == &run) checkHorizontalAnimation(true);
		else checkHorizontalAnimation();
	}

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{		
		if (position.y < SCREEN_HEIGHT - current_animation->GetCurrentFrame().h)
		{
			position.y += speed;
			modifyHorizonY();
		}
		else
		{
			current_animation = &run;
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		if (position.y > 0)
		{
			position.y -= speed;
			modifyHorizonY();
		}
		if (current_animation == &run)
		{
			checkHorizontalAnimation();
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->particles->AddParticle(App->particles->cannon, position.x - current_animation->GetCurrentFrame().w, position.y);
	}

	//if(App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE
	//   && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE)
	//	current_animation = &idle;

	// Draw everything --------------------------------------
	BlitTarget* temp = new BlitTarget(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()), nullptr, 0);
	if (destroyed == false)
		App->renderer->depthBuffer[temp->depth].push_back(*temp);

	delete temp;

	return UPDATE_CONTINUE;
}

void ModulePlayer::checkHorizontalAnimation(bool running)
{
	int realPosition = position.x + middle.GetCurrentFrame().w / 2;
	
	if (realPosition <= SCREEN_DIVISOR)
	{
		if(!running) current_animation = &left2;
		setCharSpeed();
		return;
	}
	else if (realPosition <= (SCREEN_DIVISOR * 2))
	{
		if (!running) current_animation = &left1;
		setCharSpeed();
		return;
	}
	else if (realPosition <= (SCREEN_DIVISOR * 3))
	{
		if (!running) current_animation = &middle;
		setCharSpeed();
		return;
	}
	else if (realPosition <= (SCREEN_DIVISOR * 4))
	{
		if (!running) current_animation = &right1;
		setCharSpeed();
		return;
	}
	else if (realPosition <= (SCREEN_DIVISOR * 5))
	{
		if (!running) current_animation = &right2;
		setCharSpeed();
		return;
	}
}

void ModulePlayer::modifyHorizonY()
{
	float offsetValue = (float)SCREEN_HEIGHT - (float)current_animation->GetCurrentFrame().h;
	float temp = (offsetValue - (float)position.y) / offsetValue;
	App->renderer->horizonY = (temp * ((float)FLOOR_Y_MAX - (float)FLOOR_Y_MIN)) + (float)FLOOR_Y_MIN;
}

void ModulePlayer::setCharSpeed()
{
	int screenPosition = (position.x + (run.GetCurrentFrame().w / 2)) - (SCREEN_WIDTH/2);
	float speedPercent = ((float)screenPosition * 1.0f) / (SCREEN_WIDTH/2.0f);
	if (current_animation == &middle) speedPercent = 0.0f;
	App->renderer->playerSpeed = speedPercent * 10.0f;
}

// TODO 13: Make so is the laser collides, it is removed and create an explosion particle at its position
// TODO 14: Make so if the player collides, it is removed and create few explosions at its positions
// then fade away back to the first screen (use the "destroyed" bool already created 
// You will need to create, update and destroy the collider with the player
