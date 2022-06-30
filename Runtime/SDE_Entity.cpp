#include "SDE_Entity.h"

#include "SDE_Scene.h"
#include "SDE_Director.h"
#include "SDE_MemoryPool.h"

#include <functional>
#include <unordered_set>

class SDE_Entity::Impl
{
public:
	std::string							m_strType;
	SDE_Scene*							m_pScene;
	std::unordered_set<SDE_Component*>	m_setComponent;

public:
	Impl(SDE_Scene* pScene, Def* defEntity)
	{
		m_pScene = pScene;
		m_strType = defEntity->strType;
	}
	~Impl()
	{
		for (std::unordered_set<SDE_Component*>::iterator iter = m_setComponent.begin();
			iter != m_setComponent.end(); iter++)
		{
			m_pScene->DestroyComponent(*iter);
		}
	}
};

SDE_Scene* SDE_Entity::GetScene()
{
	return m_pImpl->m_pScene;
}

SDE_Component* SDE_Entity::AddComponent(SDE_Component::Def* defComponent)
{
	SDE_Component* pComponent = m_pImpl->m_pScene->CreateComponent(defComponent);

	if (pComponent)
	{
		pComponent->m_pEntity = this;
		m_pImpl->m_setComponent.insert(pComponent);
	}
	return pComponent;
}

bool SDE_Entity::DeleteComponent(SDE_Component* pComponent)
{
	if (m_pImpl->m_setComponent.find(pComponent) == m_pImpl->m_setComponent.end())
	{
		return false;
	}
	m_pImpl->m_setComponent.erase(pComponent);

	return m_pImpl->m_pScene->DestroyComponent(pComponent);
}

SDE_Entity::SDE_Entity(SDE_Scene* pScene, SDE_Entity::Def* defEntity)
{
	SDE_MemoryPool* pMemoryPool = m_pImpl->m_pScene->GetDirector()->GetMemoryPool();
	void* pMem = pMemoryPool->Allocate(sizeof(SDE_Entity));
	m_pImpl = new (pMem) Impl(pScene, defEntity);

	for (std::list<SDE_Component::Def*>::iterator iter = defEntity->listDefComponent.begin();
		iter != defEntity->listDefComponent.end(); iter++)
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