#include "GameComponent.h"

GameEntity* GameComponent::GetEntity()
{
	return m_pEntity;
}

const std::string& GameComponent::GetType()
{
	return m_strType;
}

void GameComponent::SetType(const std::string& strType)
{
	m_strType = strType;
}

GameComponent::GameComponent(GameEntity* pEntity)
{
	m_pEntity = pEntity;
}

GameComponent::~GameComponent()
{

}