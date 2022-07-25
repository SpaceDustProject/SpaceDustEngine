#ifndef _SDE_BLACKBOARD_H_
#define _SDE_BLACKBOARD_H_

#include <string>

class SDE_Blackboard
{
public:
	// ʹ�ü���ȡָ����������ֵ
	bool GetInteger(std::string strKey, int& strValue);
	// ����ֵΪ�������͵ļ�ֵ�ԣ���ָ���Ƿ�ɸ���ԭֵ(Ĭ�Ͽ���)
	bool SetValue(std::string strKey, int strValue, bool canOverride = true);

	// ʹ�ü���ȡָ������������ֵ
	bool GetNumber(std::string strKey, float& strValue);
	// ����ֵΪ���������͵ļ�ֵ�ԣ���ָ���Ƿ�ɸ���ԭֵ(Ĭ�Ͽ���)
	bool SetValue(std::string strKey, float strValue, bool canOverride = true);

	// ʹ�ü���ȡָ����������ֵ
	bool GetBool(std::string strKey, bool& strValue);
	// ����ֵΪ�������͵ļ�ֵ�ԣ���ָ���Ƿ�ɸ���ԭֵ(Ĭ�Ͽ���)
	bool SetValue(std::string strKey, bool strValue, bool canOverride = true);

	// ʹ�ü���ȡָ���ַ�������ֵ
	bool GetString(std::string strKey, std::string& strValue);
	// ����ֵΪ�ַ������͵ļ�ֵ�ԣ���ָ���Ƿ�ɸ���ԭֵ(Ĭ�Ͽ���)
	bool SetValue(std::string strKey, std::string strValue, bool canOverride = true);

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_Blackboard();
	~SDE_Blackboard();
};

#endif // !_SDE_BLACKBOARD_H_