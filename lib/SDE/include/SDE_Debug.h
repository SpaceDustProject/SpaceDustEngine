#ifndef _SDE_DEBUG_H_
#define _SDE_DEBUG_H_

typedef void(*SDE_OutputFunc)(const char*);

class SDE_Debug
{
public:
	// 不加处理地输出信息
	void OutputOriginal(const char* strFormat, ...);

	// 输出信息
	void OutputInfo(const char* strFormat, ...);

	// 输出错误(该函数会使程序退出)
	void OutputError(const char* strFormat, ...);

	// 设置 Debug 输出函数
	void SetOuputFunc(SDE_OutputFunc funcOutputLog);

	// 设置 Debug 信息缓存大小
	void SetBufferSize(size_t nSize);

	// 设置是否输出时间
	void SetOutputTime(bool bCanOutput);

private:
	SDE_OutputFunc	m_funcOutput;
	char*			m_strBuffer;
	size_t			m_nBufferSize;

	bool			m_bCanOutputTime;	// 是否输出时间

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