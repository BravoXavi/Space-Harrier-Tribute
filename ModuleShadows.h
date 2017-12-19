#ifndef __MODULESHADOWS_H__
#define __MODULESHADOWS_H__

#include "Module.h"

class ModuleShadows : public Module
{
public:
	ModuleShadows(bool active = true);
	~ModuleShadows();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();
};

#endif // __MODULESHADOWS_H__