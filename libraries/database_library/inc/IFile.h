#pragma once

#include <memory>
#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  ����� �������� �����
*/
//---
enum class FileOpenMode
{
	Read,  ///< ������� ���� �� ������
	Write, ///< ������� ���� �� ������
};

//------------------------------------------------------------------------------
/**
  ��������� �����
*/
//---
class IFile
{
public:
	virtual ~IFile() = default;

public:
	/// �������� ��� �����
	virtual std::string GetFileName() const = 0;
	/// ������� ����
	virtual bool Open(const std::vector<FileOpenMode> & openModes = {FileOpenMode::Read,
		FileOpenMode::Write}) = 0;
	/// ������� ����
	virtual bool Close() = 0;
	/// ���������� �������� �����
	virtual std::optional<std::vector<char>> ReadBytes(size_t count) = 0;
	/// ���������� �������� �����
	virtual bool WriteBytes(const std::vector<char> & data) = 0;
};

/// ��������� �� IFile
using IFilePtr = std::shared_ptr<IFile>;
