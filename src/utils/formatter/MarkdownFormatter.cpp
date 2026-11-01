#include "MarkdownFormatter.hpp"
#include <stdexcept>

namespace
{
void AssertHeaderLevelValid(int level)
{
	if (level < 1 || level > 6)
	{
		throw std::invalid_argument("Уровень заголовка Markdown должен быть от 1 до 6");
	}
}

void AssertIsTableNotEmpty(const TableRow& headers)
{
	if (headers.empty())
	{
		throw std::invalid_argument("Таблица Markdown должна содержать хотя бы один столбец");
	}
}

void AssertIsRowWidthValid(const TableRow& headers, const TableRow& row)
{
	if (row.size() != headers.size())
	{
		throw std::invalid_argument("Количество ячеек строки не совпадает с количеством столбцов");
	}
}

std::string EscapeCell(const std::string& cell)
{
	std::string escaped;
	escaped.reserve(cell.size());

	for (const char symbol : cell)
	{
		if (symbol == '|')
		{
			escaped += '\\';
		}
		escaped += symbol;
	}

	return escaped;
}

std::string BuildTableLine(const TableRow& cells)
{
	std::string line = "|";
	for (const auto& cell : cells)
	{
		line += " " + EscapeCell(cell) + " |";
	}

	return line + "\n";
}

std::string BuildSeparatorLine(const std::size_t columnCount)
{
	std::string line = "|";
	for (std::size_t index = 0; index < columnCount; ++index)
	{
		line += " --- |";
	}

	return line + "\n";
}
} // namespace

MarkdownFormatter::MarkdownFormatter() = default;

MarkdownFormatter::~MarkdownFormatter() = default;

void MarkdownFormatter::AddHeader(int level, const std::string& text)
{
	AssertHeaderLevelValid(level);

	m_buffer += std::string(static_cast<size_t>(level), '#');
	m_buffer += " ";
	m_buffer += text;
	m_buffer += "\n\n";
}

void MarkdownFormatter::AddCodeBlock(const std::string& language, const std::string& code)
{
	m_buffer += "```" + language + "\n";
	m_buffer += code;

	if (!code.empty() && code.back() != '\n')
	{
		m_buffer += "\n";
	}

	m_buffer += "```\n\n";
}

void MarkdownFormatter::AddText(const std::string& text)
{
	m_buffer += text;
	m_buffer += "\n\n";
}

void MarkdownFormatter::AddTree(const std::string& treeContent)
{
	m_buffer += "```text\n";
	m_buffer += treeContent;
	m_buffer += "\n```\n\n";
}

void MarkdownFormatter::AddBulletList(const std::vector<std::string>& items)
{
	for (const auto& item : items)
	{
		m_buffer += "- " + item + "\n";
	}

	m_buffer += "\n";
}

void MarkdownFormatter::AddTable(const TableRow& headers, const std::vector<TableRow>& rows)
{
	AssertIsTableNotEmpty(headers);

	m_buffer += BuildTableLine(headers);
	m_buffer += BuildSeparatorLine(headers.size());

	for (const auto& row : rows)
	{
		AssertIsRowWidthValid(headers, row);
		m_buffer += BuildTableLine(row);
	}

	m_buffer += "\n";
}

std::string MarkdownFormatter::Build() const
{
	return m_buffer;
}
