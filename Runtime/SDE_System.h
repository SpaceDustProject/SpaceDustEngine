#ifndef _SDE_SYSTEM_H_
#define _SDE_SYSTEM_H_

#include <lua.hpp>

class SDE_System
{
public:
	static const luaL_Reg* GetMemberFunc();
	static lua_CFunction GetGCFunc();

private:
	lua_Integer m_nID;

private:
	SDE_System();
	~SDE_System();

	friend class SDE_Scene;
};

#endif // !_SDE_SYSTEM_H_