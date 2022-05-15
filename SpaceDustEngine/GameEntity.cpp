#include "GameEntity.h"
#include "MemoryManager.h"

#include <new>

class GameEntity::Impl
{
public:
	// 实体所属场景
	GameScene* m_pScene;

	// 实体间的父子关系
	GameEntity* m_pParent;
	GameEntity* m_pHead;
	GameEntity* m_pPrev;
	GameEntity* m_pNext;
};

GameEntity::GameEntity()
{
	void* pMem = MemoryManager::Instance().Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl();
}

GameEntity::~GameEntity()
{
	m_pImpl->~Impl();
	MemoryManager::Instance().Free(m_pImpl, sizeof(Impl));
}