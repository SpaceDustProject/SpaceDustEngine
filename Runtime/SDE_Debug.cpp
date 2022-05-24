#include "SDE_Debug.h"

#include <stdio.h>
#include <stdarg.h>

SDE_Debug::SDE_Debug()
{
	// 默认日志输出函数
	m_funcOutputLog = [](const char* strFormat, ...)->void
	{
		va_list listArgs;
		va_start(listArgs, strFormat);
		vprintf_s(strFormat, listArgs);
		va_end(listArgs);
	};
}

void SDE_Debug::OutputLog(const char* strFormat, ...)
{
	va_list listArgs;
	va_start(listArgs, strFormat);
	m_funcOutputLog(strFormat, listArgs);
	va_end(listArgs);
}

void SDE_Debug::SetLogOuputFunc(SDE_OutputFunc funcOutputLog)
{
	m_funcOutputLog = funcOutputLog;
}