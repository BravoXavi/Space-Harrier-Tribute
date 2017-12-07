#ifndef __MODULETIME_H__
#define __MODULETIME_H__

#include "Module.h"

class ModuleTime : public Module
{
public:
	ModuleTime(bool active = true);
	~ModuleTime();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	float getDeltaTime() const;

private:
	float deltaTime = 0.0f;
	float lastDeltaTime = 0.0f;

};

#endif // __MODULETIME_H__