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
#include "ModuleTime.h"

const float ModulePlayer::playerDepth = 0.0f;

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
	position.x = (float)(SCREEN_WIDTH / 2) - (current_animation->GetCurrentFrame().w / 2);
	position.y = (float)(SCREEN_HEIGHT - current_animation->GetCurrentFrame().h);

	collider = App->collision->AddCollider({ (int)position.x * SCREEN_SIZE, (int)position.y * SCREEN_SIZE, current_animation->GetCurrentFrame().w * SCREEN_SIZE, current_animation->GetCurrentFrame().h * SCREEN_SIZE }, PLAYER, (int)playerDepth, App->player);

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
	float speed = 200.0f * App->time->getDeltaTime();

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (position.x > 0.0f) position.x -= speed;
		if (current_animation == &run) checkHorizontalAnimation(true);
		else checkHorizontalAnimation();

		moveCollider();
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (position.x + (float)current_animation->GetCurrentFrame().w < (float)SCREEN_WIDTH) position.x += speed;
		if (current_animation == &run) checkHorizontalAnimation(true);
		else checkHorizontalAnimation();

		moveCollider();
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
		
		moveCollider();
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

		moveCollider();
	}

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->particles->AddParticle(App->particles->p_laser, position.x + (2.0f *(float)current_animation->GetCurrentFrame().w)/2.0f, position.y + (float)current_animation->GetCurrentFrame().h/3.0f , 1.0f, P_LASER);
	}

	// Draw everything --------------------------------------
	BlitTarget* temp = new BlitTarget(graphics, position.x, position.y, playerDepth, &(current_animation->GetCurrentFrame()), nullptr);

	if (destroyed == false)
		App->renderer->depthBuffer[(int)temp->z].push_back(*temp);

	delete temp;

	return UPDATE_CONTINUE;
}

void ModulePlayer::moveCollider() const
{
	collider->SetPos((int)position.x, (int)position.y, (int)playerDepth);
	collider->SetSize(current_animation->GetCurrentFrame().w, current_animation->GetCurrentFrame().h);
}

void ModulePlayer::checkHorizontalAnimation(bool running)
{
	int realPosition = (int)position.x + middle.GetCurrentFrame().w / 2;
	
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

void ModulePlayer::modifyHorizonY() const
{
	float offsetValue = (float)SCREEN_HEIGHT - (float)current_animation->GetCurrentFrame().h;
	float temp = (offsetValue - (float)position.y) / offsetValue;
	App->renderer->horizonY = (temp * ((float)FLOOR_Y_MAX - (float)FLOOR_Y_MIN)) + (float)FLOOR_Y_MIN;
}

void ModulePlayer::setCharSpeed()
{
	int screenPosition = ((int)position.x + (run.GetCurrentFrame().w / 2)) - (SCREEN_WIDTH / 2);
	float speedPercent = ((float)screenPosition * 1.0f) / ((float)SCREEN_WIDTH / 2.0f);
	if (current_animation == &middle) speedPercent = 0.0f;
	App->renderer->playerSpeed = (speedPercent * 150.0f) * App->time->getDeltaTime();
}

bool ModulePlayer::onCollision(Collider* c1, Collider* c2) 
{
	LOG("Player Collision");
	return true;
}
