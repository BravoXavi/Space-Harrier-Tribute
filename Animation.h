#pragma once
#include <vector>
#include "ModuleTime.h"
#include "Application.h"

using namespace std;

class Animation
{
public:
	bool loop = true;
	bool animationWithoutLoopEnded = false;
	bool reverseAnimation = false;
	float speed = 1.0f;
	vector<SDL_Rect> frames;

private:
	float current_frame = 0.0f;
	int loops = 0;

public:
	Animation()
	{}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), frames(anim.frames)
	{}

	SDL_Rect& GetCurrentFrame()
	{
		float last_frame = (float) frames.size();

		if (!reverseAnimation) current_frame += speed*App->time->getDeltaTime();
		else current_frame -= speed*App->time->getDeltaTime();

		if (current_frame >= last_frame)
		{
			LOG("Speed = %f", speed*App->time->getDeltaTime());
			if (!loop) animationWithoutLoopEnded = true;
			current_frame = (loop) ? 0.0f : MAX(last_frame - 1.0f, 0.0f);
			loops++;
		} 

		if (current_frame < 0.0f) current_frame = 0.0f;

		return frames[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0.0f;
	}
};