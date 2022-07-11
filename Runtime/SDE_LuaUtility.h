#ifndef SDE_LUA_UTILITY_H_
#define SDE_LUA_UTILITY_H_

#define SDE_API extern "C" int

#define SDE_LUA_FUNC_X(FUNC_NAME, STATE_NAME) \
		SDE_API FUNC_NAME(lua_State* STATE_NAME)

#define SDE_LUA_FUNC(FUNC_NAME) SDE_LUA_FUNC_X(FUNC_NAME, pState)

#define SDE_NAME_GLOBAL		"SDE_Global"
#define SDE_NAME_REGISTRY	"SDE_Registry"
#define SDE_NAME_RUNTIME	"SDE_Runtime"
#define SDE_NAME_TEMPORARY	"SDE_Temporary"

#include <lua.hpp>
#include <list>
#include <string>
#include <initializer_list>

typedef int (*SDE_LuaFunc) (lua_State* L);

class SDE_Data;
class SDE_LuaLightMetatable;

struct SDE_LuaReg
{
	const char* name;
	SDE_LuaFunc func;
};

class SDE_LuaPackage
{
public:
	const std::list<SDE_LuaReg>& GetFuncList() const {
		return m_listFunc;
	}

private:
	std::list<SDE_LuaReg> m_listFunc;

public:
	SDE_LuaPackage(const std::initializer_list<SDE_LuaReg>& listFunc) :
		m_listFunc(listFunc) {}

	~SDE_LuaPackage() = default;
};

// Ϊ light userdata ������������Ķ����ṩԪ��
class SDE_LuaLightMetatable
{
public:
	const std::string& GetType() const {
		return m_strType;
	}

	const SDE_LuaPackage& GetPackage() const {
		return m_packageFunc;
	}

private:
	std::string		m_strType;
	SDE_LuaPackage	m_packageFunc;

public:
	SDE_LuaLightMetatable(
		const std::string& strType,
		const SDE_LuaPackage& packageFunc
	) :
		m_strType(strType), m_packageFunc(packageFunc) {}

	~SDE_LuaLightMetatable() = default;
};

// Ϊ full userdata �� table ���� lua ����Ķ����ṩԪ��
class SDE_LuaMetatable
{
public:
	const std::string& GetType() const {
		return m_strType;
	}

	const SDE_LuaPackage& GetPackage() const {
		return m_packageFunc;
	}

	SDE_LuaFunc GetGCFunc() const {
		return m_funcGC;
	}

private:
	std::string		m_strType;
	SDE_LuaPackage	m_packageFunc;
	SDE_LuaFunc		m_funcGC;

public:
	SDE_LuaMetatable(
		const std::string& strType,
		const SDE_LuaPackage& packageFunc,
		SDE_LuaFunc funcGC = nullptr
	) :
		m_strType(strType), m_packageFunc(packageFunc), m_funcGC(funcGC) {}

	~SDE_LuaMetatable() = default;
};

namespace SDE_LuaUtility
{
	// ��ָ��������Ϊ�ű�����
	bool RunScript(lua_State* pState, const SDE_Data& dataScript);

	// ��ȡ SDE ȫ�ֱ�
	void GetGlobal(lua_State* pState);

	// ��ȡ SDE ȫ�ֱ��е�ע���
	void GetRegistry(lua_State* pState);

	// ��� SDE ȫ�ֱ��е����л���
	void GetRuntime(lua_State* pState);

	// ���ָ��������ȫ�ֱ��е����л���
	void GetRuntime(lua_State* pState, const std::string& strName);

	// ��� SDE ȫ�ֱ��е���ʱ��
	void GetTemporary(lua_State* pState);
	
	// �����еĺ���ע�ᵽջ���ı���
	void SetPackage(lua_State* pState, const SDE_LuaPackage& package);

	// ��Ԫ��ע�ᵽȫ��
	void RegisterMetatable(lua_State* pState, const SDE_LuaMetatable& metatable);

	// ��ȡ�����û����ݲ��鿴������
	void* GetLightUserdata(lua_State* pState, int nIndex, const std::string& strName);

	// �鿴��ջ���
	void CheckStack(lua_State* pState);

	bool AddPath(lua_State* pState, const std::string& strPath);
	bool DeletePath(lua_State* pState, const std::string& strPath);

	bool AddCPath(lua_State* pState, const std::string& strCPath);
	bool DeleteCPath(lua_State* pState, const std::string& strCPath);
};

#endif // !SDE_LUA_UTILITY_H_