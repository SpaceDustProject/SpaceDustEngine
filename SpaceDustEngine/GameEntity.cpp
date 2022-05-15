#include "GameEntity.h"
#include "MemoryManager.h"

#include <new>

class GameEntity::Impl
{
public:
	// ʵ����������
	GameScene* m_pScene;

	// ʵ���ĸ��ӹ�ϵ
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