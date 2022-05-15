#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "GameEntity.h"
#include "GameSystem.h"

class GameScene
{
public:
	// ���һ��ʵ��
	GameEntity* AddEntity();
	// ɾ��һ��ʵ��
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