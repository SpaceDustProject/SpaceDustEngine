#ifndef _GAME_ENTITY_H_
#define _GAME_ENTITY_H_

#include "GameComponent.h"

#include <string>
#include <initializer_list>

class GameScene;

class GameEntity
{
public:

private:
	class Impl;
	Impl* m_pImpl;

private:
	GameEntity();
	~GameEntity();
};

#endif // !_GAME_ENTITY_H_