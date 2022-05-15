#ifndef _GAME_COMPONENT_H_
#define _GAME_COMPONENT_H_

#include <string>

class GameEntity;

class GameComponent
{
public:
	// ��ȡ�������ʵ��
	GameEntity* GetEntity();

	// ��ȡ���������
	const std::string& GetType();
	// �������������
	void SetType(const std::string& strName);

private:
	std::string m_strType;	// ���������
	GameEntity* m_pEntity;	// �������ʵ��

private:
	GameComponent(GameEntity* pEntity);
	~GameComponent();
};

#endif // !_GAME_COMPONENT_H_