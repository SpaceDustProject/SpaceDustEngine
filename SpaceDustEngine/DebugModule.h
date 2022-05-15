#ifndef _DEBUG_MODULE_H_
#define _DEBUG_MODULE_H_

class DebugModule
{
public:
	void Output(const char* strFormat, ...);

public:
	~DebugModule() = default;
	DebugModule(const DebugModule&) = delete;
	DebugModule& operator=(const DebugModule&) = delete;
	static DebugModule& Instance()
	{
		static DebugModule instance;
		return instance;
	}

private:
	DebugModule() = default;
};

#endif // !_DEBUG_MODULE_H_