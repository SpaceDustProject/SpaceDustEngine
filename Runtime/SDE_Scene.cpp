#include "SDE_Scene.h"
#include "SDE_MemoryPool.h"

#include <unordered_map>

class SDE_Scene::Impl
{
public:
	std::unordered_map<lua_Integer, SDE_Entity*> m_mapEntity;
	std::unordered_map<lua_Integer, SDE_System*> m_mapSystem;

public:
	Impl()
	{

	}
};

SDE_Scene::SDE_Scene()
{
	void* pMem = g_memoryPool.Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl();
}

SDE_Scene::~SDE_Scene()
{
	m_pImpl->~Impl();
	g_memoryPool.Free(m_pImpl, sizeof(Impl));
}