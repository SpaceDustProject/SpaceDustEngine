#ifndef _SDE_LUA_LIGHT_USERDATA_H_
#define _SDE_LUA_LIGHT_USERDATA_H_

#include <string>

#define SDE_TYPE_LIGHTUSERDATA	"SDE_LightUserdata"

class SDE_LuaPackage;

struct SDE_LuaLightUserdataDef
{
	std::string	strName;
	bool bIsRegistered;

	SDE_LuaLightUserdataDef(const std::string& strNameIn)
		: strName(strNameIn), bIsRegistered(false) {}

	virtual ~SDE_LuaLightUserdataDef() {}
};

class SDE_LuaLightUserdata
{
public:
	const std::string& GetTypeLU() const {
		return m_strTypeLU;
	}

	const std::string& GetName() const {
		return m_strName;
	}

	void SetName(const std::string& strName) {
		m_strName = strName;
	}

protected:
	std::string m_strTypeLU;
	std::string m_strName;

public:
	SDE_LuaLightUserdata(const std::string& strTypeLU)
		: m_strTypeLU(strTypeLU) {}

	virtual ~SDE_LuaLightUserdata() = default;
};

extern SDE_LuaPackage g_packageLightUserdata;

#endif // !_SDE_LUA_LIGHT_USERDATA_H_