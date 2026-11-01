#pragma once

#include <string>
#include <vector>

using TableRow = std::vector<std::string>;

class MarkdownFormatter
{
public:
	MarkdownFormatter();
	~MarkdownFormatter();

	void AddHeader(int level, const std::string& text);
	void AddCodeBlock(const std::string& language, const std::string& code);
	void AddText(const std::string& text);
	void AddTree(const std::string& treeContent);
	void AddBulletList(const std::vector<std::string>& items);
	void AddTable(const TableRow& headers, const std::vector<TableRow>& rows);

	[[nodiscard]] std::string Build() const;

private:
	std::string m_buffer;
};
