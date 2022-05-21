#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <lua.hpp>

#define SDE_API extern "C"

class VirtualMachine
{
public:
	// ��ȡȫ�������״̬
	static lua_State* GetLuaState();

	// ���Ԥ������
	static void AddPreloadFunc(lua_State* pLuaVM, const char* strName, lua_CFunction funcPreload);

	// ��ȡ���� index �� table �ĳ���
	static int GetTableLength(lua_State* pLuaVM, int nIndex);

	// ǳ����ָ�� index �ϵ� table ������ջ
	static void CopyTable(lua_State* pLuaVM, int nIndex);

public:
	~VirtualMachine();
	VirtualMachine(const VirtualMachine&) = delete;
	VirtualMachine& operator=(const VirtualMachine&) = delete;
	static VirtualMachine& Instance()
	{
		static VirtualMachine instance;
		return instance;
	}

private:
	VirtualMachine();
};

#endif // !_VIRTUAL_MACHINE_H_