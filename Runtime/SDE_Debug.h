#ifndef _SDE_DEBUG_H_
#define _SDE_DEBUG_H_

typedef void(*SDE_OutputFunc)(const char*, ...);

class SDE_Debug
{
public:
	// 输出日志
	void OutputLog(const char* strFormat, ...);

	// 设置日志输出函数
	void SetLogOuputFunc(SDE_OutputFunc funcOutputLog);

private:
	SDE_OutputFunc m_funcOutputLog;

public:
	~SDE_Debug() = default;
	SDE_Debug(const SDE_Debug&) = delete;
	SDE_Debug& operator=(const SDE_Debug&) = delete;
	static SDE_Debug& Instance()
	{
		static SDE_Debug instance;
		return instance;
	}
private:
	SDE_Debug();
};

#endif // !_SDE_DEBUG_H_