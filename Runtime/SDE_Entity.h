#ifndef _SDE_ENTITY_H_
#define _SDE_ENTITY_H_

#include <lua.hpp>

class SDE_Entity
{
public:
	static const luaL_Reg* GetMemberFunc();
	static lua_CFunction GetGCFunc();

private:
	lua_Integer m_nID;

private:
	SDE_Entity();
	~SDE_Entity();

	friend class SDE_Scene;
};

#endif // !_SDE_ENTITY_H_