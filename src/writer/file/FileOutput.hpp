#pragma once

#include "writer/IOutputTarget.hpp"
#include <filesystem>

class FileOutput final : public IOutputTarget
{
public:
	explicit FileOutput(std::filesystem::path outputPath);
	~FileOutput() override;

	void Write(const std::string& content) override;

private:
	std::filesystem::path m_outputPath;
};
