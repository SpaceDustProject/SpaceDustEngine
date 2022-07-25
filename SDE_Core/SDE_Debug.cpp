#include "SDE_Debug.h"

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <functional>

// 在时间为个位数时前缀加上 "0" 变为时钟风格
std::string SDE_Debug_ToClock(int num)
{
	std::string strOutput = std::to_string(num);
	if (num < 10) strOutput = "0" + strOutput;
	return strOutput;
}

// 在调试信息前加上时间
std::string SDE_Debug_AppendTime(const std::string& strInput)
{
	std::string strOutput = strInput;

	time_t timeNow = time(0);
	tm timeLocal;
	localtime_s(&timeLocal, &timeNow);

	strOutput =
		"[" + SDE_Debug_ToClock(timeLocal.tm_hour) +
		":" + SDE_Debug_ToClock(timeLocal.tm_min) +
		":" + SDE_Debug_ToClock(timeLocal.tm_sec) +
		"] " + strInput;

	return strOutput;
}

void SDE_Debug::OutputOriginal(const char* strFormat, ...)
{
	va_list listArgs;
	va_start(listArgs, strFormat);
	vsnprintf(m_strBuffer, m_nBufferSize, strFormat, listArgs);
	va_end(listArgs);

	m_funcOutput(m_strBuffer);
}

void SDE_Debug::OutputInfo(const char* strFormat, ...)
{
	va_list listArgs;
	va_start(listArgs, strFormat);
	vsnprintf(m_strBuffer, m_nBufferSize, strFormat, listArgs);
	va_end(listArgs);

	std::string strOutput = m_strBuffer;
	strOutput = "[INFO]: " + strOutput;
	
	if (m_bCanOutputTime)
	{
		strOutput = SDE_Debug_AppendTime(strOutput);
	}

	strcpy_s(m_strBuffer, m_nBufferSize, strOutput.c_str());

	m_funcOutput(m_strBuffer);
}

void SDE_Debug::OutputError(const char* strFormat, ...)
{
	va_list listArgs;
	va_start(listArgs, strFormat);
	vsnprintf(m_strBuffer, m_nBufferSize, strFormat, listArgs);
	va_end(listArgs);

	std::string strOutput = m_strBuffer;
	strOutput = "[ERROR]: " + strOutput;

	if (m_bCanOutputTime)
	{
		strOutput = SDE_Debug_AppendTime(strOutput);
	}

	strcpy_s(m_strBuffer, m_nBufferSize, strOutput.c_str());

	m_funcOutput(m_strBuffer);
	abort();
}

void SDE_Debug::SetOuputFunc(SDE_OutputFunc funcOutputLog)
{
	m_funcOutput = funcOutputLog;
}

void SDE_Debug::SetBufferSize(size_t nSize)
{
	// 删除原来的缓存区
	delete[] m_strBuffer;

	// 申请新的缓存区
	m_nBufferSize = nSize;
	m_strBuffer = new char[nSize];
}

void SDE_Debug::SetOutputTime(bool bCanOutputTime)
{
	m_bCanOutputTime = bCanOutputTime;
}

SDE_Debug::SDE_Debug()
{
	// 默认缓存区大小 1024 字节
	m_nBufferSize = 1024;

	m_strBuffer = new char[m_nBufferSize];

	// 默认日志输出函数(输出到控制台)
	m_funcOutput = [](const char* strContent)->void
	{
		printf_s(strContent);
	};

	// 默认输出时间
	m_bCanOutputTime = true;
}