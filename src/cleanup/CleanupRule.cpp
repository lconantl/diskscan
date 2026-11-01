#include "CleanupRule.hpp"

std::string DescribeMethod(const CleanupMethod method)
{
	switch (method)
	{
	case CleanupMethod::Safe: return "Можно удалить";
	case CleanupMethod::SystemTool: return "Штатным средством";
	case CleanupMethod::Review: return "Проверить вручную";
	case CleanupMethod::Keep: return "Не трогать";
	}

	return "Проверить вручную";
}
