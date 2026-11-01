#include "FileOutput.hpp"
#include "utils/text/TextConversion.hpp"
#include <fstream>
#include <stdexcept>

namespace
{
void AssertIsParentDirectoryValid(const std::filesystem::path& path)
{
	std::error_code error;
	if (!std::filesystem::is_directory(path.parent_path(), error))
	{
		throw std::runtime_error("Папка для сохранения отчёта не существует: " + ToUtf8(path));
	}
}

void AssertFileStreamOpen(const std::ofstream& stream, const std::filesystem::path& path)
{
	if (!stream.is_open())
	{
		throw std::runtime_error("Не удалось открыть для записи файл " + ToUtf8(path));
	}
}

void AssertIsWriteSucceeded(const std::ofstream& stream, const std::filesystem::path& path)
{
	if (!stream.good())
	{
		throw std::runtime_error("Не удалось записать отчёт в файл " + ToUtf8(path));
	}
}
} // namespace

FileOutput::FileOutput(std::filesystem::path outputPath)
	: m_outputPath(std::move(outputPath))
{
	AssertIsParentDirectoryValid(m_outputPath);
}

FileOutput::~FileOutput() = default;

void FileOutput::Write(const std::string& content)
{
	std::ofstream file(m_outputPath, std::ios::out | std::ios::binary | std::ios::trunc);
	AssertFileStreamOpen(file, m_outputPath);

	file.write(content.c_str(), static_cast<std::streamsize>(content.size()));
	AssertIsWriteSucceeded(file, m_outputPath);
}
