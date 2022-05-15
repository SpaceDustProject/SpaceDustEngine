#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "GameEntity.h"
#include "GameSystem.h"

class GameScene
{
public:
	// 添加一个实体
	GameEntity* AddEntity();
	// 删除一个实体
	void DeleteEntity(GameEntity* pEntity);

	GameSystem* AddSystem(const GameSystem::Def& defSystem);

private:
	class Impl;
	Impl* m_pImpl;

public:
	GameScene();
	~GameScene();
};

#endif // !_GAME_SCENE_H_