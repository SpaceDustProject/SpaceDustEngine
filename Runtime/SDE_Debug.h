#ifndef _SDE_DEBUG_H_
#define _SDE_DEBUG_H_

typedef void(*SDE_OutputFunc)(const char*);

class SDE_Debug
{
public:
	// ���Ӵ���������Ϣ
	void OutputOriginal(const char* strFormat, ...);

	// �����Ϣ
	void OutputInfo(const char* strFormat, ...);

	// �������(�ú�����ʹ�����˳�)
	void OutputError(const char* strFormat, ...);

	// ���� Debug �������
	void SetOuputFunc(SDE_OutputFunc funcOutputLog);

	// ���� Debug ��Ϣ�����С
	void SetBufferSize(size_t nSize);

	// �����Ƿ����ʱ��
	void SetOutputTime(bool bCanOutput);

private:
	SDE_OutputFunc	m_funcOutput;
	char*			m_strBuffer;
	size_t			m_nBufferSize;

	bool			m_bCanOutputTime;	// �Ƿ����ʱ��

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