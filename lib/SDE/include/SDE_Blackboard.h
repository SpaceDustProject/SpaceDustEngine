#ifndef _SDE_BLACKBOARD_H_
#define _SDE_BLACKBOARD_H_

#include <string>

class SDE_Blackboard
{
public:
	// 使用键获取指定整数类型值
	bool GetInteger(std::string strKey, int& strValue);
	// 设置值为整数类型的键值对，并指定是否可覆盖原值(默认可以)
	bool SetValue(std::string strKey, int strValue, bool canOverride = true);

	// 使用键获取指定浮点数类型值
	bool GetNumber(std::string strKey, float& strValue);
	// 设置值为浮点数类型的键值对，并指定是否可覆盖原值(默认可以)
	bool SetValue(std::string strKey, float strValue, bool canOverride = true);

	// 使用键获取指定布尔类型值
	bool GetBool(std::string strKey, bool& strValue);
	// 设置值为布尔类型的键值对，并指定是否可覆盖原值(默认可以)
	bool SetValue(std::string strKey, bool strValue, bool canOverride = true);

	// 使用键获取指定字符串类型值
	bool GetString(std::string strKey, std::string& strValue);
	// 设置值为字符串类型的键值对，并指定是否可覆盖原值(默认可以)
	bool SetValue(std::string strKey, std::string strValue, bool canOverride = true);

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_Blackboard();
	~SDE_Blackboard();
};

#endif // !_SDE_BLACKBOARD_H_