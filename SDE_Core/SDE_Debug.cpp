#include "SDE_Debug.h"

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <functional>

// ��ʱ��Ϊ��λ��ʱǰ׺���� "0" ��Ϊʱ�ӷ��
std::string SDE_Debug_ToClock(int num)
{
	std::string strOutput = std::to_string(num);
	if (num < 10) strOutput = "0" + strOutput;
	return strOutput;
}

// �ڵ�����Ϣǰ����ʱ��
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
	// ɾ��ԭ���Ļ�����
	delete[] m_strBuffer;

	// �����µĻ�����
	m_nBufferSize = nSize;
	m_strBuffer = new char[nSize];
}

void SDE_Debug::SetOutputTime(bool bCanOutputTime)
{
	m_bCanOutputTime = bCanOutputTime;
}

SDE_Debug::SDE_Debug()
{
	// Ĭ�ϻ�������С 1024 �ֽ�
	m_nBufferSize = 1024;

	m_strBuffer = new char[m_nBufferSize];

	// Ĭ����־�������(���������̨)
	m_funcOutput = [](const char* strContent)->void
	{
		printf_s(strContent);
	};

	// Ĭ�����ʱ��
	m_bCanOutputTime = true;
}