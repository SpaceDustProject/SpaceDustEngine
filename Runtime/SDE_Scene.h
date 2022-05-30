#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#include "SDE_Entity.h"
#include "SDE_System.h"

class SDE_Scene
{
public:
	bool OnInit();
	bool OnUpdate();
	bool OnQuit();

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Scene();
	~SDE_Scene();
};

#endif // !_SDE_SCENE_H_