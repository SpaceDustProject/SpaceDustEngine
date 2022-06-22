#include "SDE_Entity.h"

#include "SDE_Scene.h"
#include "SDE_Director.h"
#include "SDE_MemoryPool.h"

#include <functional>
#include <unordered_set>

class SDE_Entity::Impl
{
public:
	lua_Integer						m_nID;
	SDE_Scene*						m_pScene;
	std::unordered_set<lua_Integer>	m_setComponent;

public:
	Impl(SDE_Scene* pScene, const SDE_Entity::Def& defEntity)
	{
		m_pScene = pScene;
	}
	~Impl()
	{
		for (std::unordered_set<lua_Integer>::iterator iter = m_setComponent.begin();
			iter != m_setComponent.end(); iter++)
		{
			m_pScene->DeleteComponent(*iter);
		}
	}
};

lua_Integer SDE_Entity::GetID()
{
	return m_pImpl->m_nID;
}

SDE_Scene* SDE_Entity::GetScene()
{
	return m_pImpl->m_pScene;
}

lua_Integer SDE_Entity::AddComponent(const SDE_Component::Def& defComponent)
{
	lua_Integer nID = m_pImpl->m_pScene->AddComponent(m_pImpl->m_nID, defComponent);

	if (nID >= 0)
	{
		m_pImpl->m_setComponent.insert(nID);
	}
	return nID;
}

bool SDE_Entity::DeleteComponent(lua_Integer nID)
{
	if (m_pImpl->m_setComponent.find(nID) == m_pImpl->m_setComponent.end())
	{
		return false;
	}
	return m_pImpl->m_pScene->DeleteComponent(nID);
}

void SDE_Entity::SetID(lua_Integer nID)
{
	m_pImpl->m_nID = nID;
}

SDE_Entity::SDE_Entity(SDE_Scene* pScene, const SDE_Entity::Def& defEntity)
{
	SDE_MemoryPool* pMemoryPool = m_pImpl->m_pScene->GetDirector()->GetMemoryPool();
	void* pMem = pMemoryPool->Allocate(sizeof(SDE_Entity));
	m_pImpl = new (pMem) Impl(pScene, defEntity);

	for (std::initializer_list<SDE_Component::Def>::iterator iter = defEntity.listDefComponent.begin();
		iter != defEntity.listDefComponent.end(); iter++)
	{
		AddComponent(*iter);
	}
}

SDE_Entity::~SDE_Entity()
{
	SDE_MemoryPool* pMemoryPool = m_pImpl->m_pScene->GetDirector()->GetMemoryPool();
	m_pImpl->~Impl();
	pMemoryPool->Free(m_pImpl, sizeof(SDE_Entity));
}

luaL_Reg g_funcEntityMember[] =
{
	{nullptr, nullptr}
};

luaL_Reg g_funcEntityDefMember[] =
{
	{nullptr, nullptr}
};