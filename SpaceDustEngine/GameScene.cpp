#include "GameScene.h"
#include "MemoryManager.h"

#include <new>

class GameScene::Impl
{

};

GameScene::GameScene()
{
	void* pMem = MemoryManager::Instance().Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl();
}

GameScene::~GameScene()
{
	m_pImpl->~Impl();
	MemoryManager::Instance().Free(m_pImpl, sizeof(Impl));
}