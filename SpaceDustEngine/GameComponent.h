#ifndef _GAME_COMPONENT_H_
#define _GAME_COMPONENT_H_

#include <string>

class GameEntity;

class GameComponent
{
public:
	// 获取组件所属实体
	GameEntity* GetEntity();

	// 获取组件的类型
	const std::string& GetType();
	// 设置组件的类型
	void SetType(const std::string& strName);

private:
	std::string m_strType;	// 组件的类型
	GameEntity* m_pEntity;	// 组件所属实体

private:
	GameComponent(GameEntity* pEntity);
	~GameComponent();
};

#endif // !_GAME_COMPONENT_H_