#ifndef _SDE_FILE_IO_H_
#define _SDE_FILE_IO_H_

#include <string>

class SDE_Data;

class SDE_FileIO
{
public:
	// ��ȡָ���ļ����� SDE_Data ����
	SDE_Data Input(const std::string& strFileName, bool isBinary = true);

	// �� SDE_Data ����дΪָ���ļ�
	void Output(const std::string& strFileName, const SDE_Data& dataWritten, bool isBinary = true);

private:
	class Impl;
	Impl* m_pImpl;

public:
	~SDE_FileIO();
	SDE_FileIO(const SDE_FileIO&) = delete;
	SDE_FileIO& operator=(const SDE_FileIO&) = delete;
	static SDE_FileIO& Instance()
	{
		static SDE_FileIO instance;
		return instance;
	}
private:
	SDE_FileIO();
};

#endif // !_SDE_FILE_IO_H_