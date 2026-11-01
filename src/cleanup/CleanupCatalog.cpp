#include "CleanupCatalog.hpp"
#include <iterator>

namespace
{
using enum CleanupMethod;

std::vector<CleanupRule> BuildSystemFileRules()
{
	return {
		{"Файл гибернации",
			"hiberfil.sys",
			SystemTool,
			"powercfg /h off (или /h /type reduced) от администратора",
			TargetKind::File},
		{"Файл подкачки",
			"pagefile.sys",
			Keep,
			"Не удалять; можно уменьшить в настройках виртуальной памяти",
			TargetKind::File},
		{"Файл подкачки UWP", "swapfile.sys", Keep, "Управляется системой", TargetKind::File},
		{"Полный дамп памяти",
			"Windows/MEMORY.DMP",
			Safe,
			"Удалить, если не нужен для отладки BSOD",
			TargetKind::File},
	};
}

std::vector<CleanupRule> BuildWindowsRules()
{
	return {
		{"Временные файлы Windows", "Windows/Temp", Safe, "Очистить содержимое"},
		{"Кэш Центра обновления",
			"Windows/SoftwareDistribution/Download",
			SystemTool,
			"Очистка диска → Очистка системных файлов"},
		{"Хранилище компонентов WinSxS",
			"Windows/WinSxS",
			SystemTool,
			"DISM /Online /Cleanup-Image /StartComponentCleanup; размер завышен жёсткими ссылками"},
		{"Предыдущая установка Windows",
			"Windows.old",
			SystemTool,
			"Очистка диска → «Предыдущие установки Windows»"},
		{"Остатки обновления Windows",
			"$WINDOWS.~BT",
			SystemTool,
			"Очистка диска → «Временные файлы установки Windows»"},
		{"Остатки обновления Windows",
			"$WINDOWS.~WS",
			SystemTool,
			"Очистка диска → «Временные файлы установки Windows»"},
		{"Остатки обновления Windows", "ESD", SystemTool, "Очистка диска"},
		{"Корзина", "$Recycle.Bin", Safe, "Очистить корзину"},
		{"Кэш оптимизации доставки",
			"Windows/ServiceProfiles/NetworkService/AppData/Local/Microsoft/Windows/"
			"DeliveryOptimization",
			SystemTool,
			"Очистка диска → «Файлы оптимизации доставки»"},
		{"Отчёты ядра", "Windows/LiveKernelReports", Safe, "Удалить *.dmp"},
		{"Мини-дампы BSOD", "Windows/Minidump", Safe, "Удалить"},
		{"Журналы Windows", "Windows/Logs", Review, "CBS-логи можно удалить"},
		{"Кэш установщиков MSI",
			"Windows/Installer",
			Keep,
			"Не удалять вручную — сломает обновление и удаление программ"},
		{"Хранилище драйверов",
			"Windows/System32/DriverStore/FileRepository",
			Review,
			"Старые версии драйверов удалять через pnputil / DriverStore Explorer"},
		{"Prefetch", "Windows/Prefetch", Keep, "Мизерный выигрыш, не трогать"},
	};
}

std::vector<CleanupRule> BuildProgramDataRules()
{
	return {
		{"Кэш установщиков (VS и др.)",
			"ProgramData/Package Cache",
			Keep,
			"Не удалять вручную — нужен для изменения и удаления программ"},
		{"Отчёты об ошибках (WER)", "ProgramData/Microsoft/Windows/WER", Safe, "Удалить"},
		{"Docker (данные)", "ProgramData/Docker", Review, "docker system prune -a"},
		{"Docker Desktop", "ProgramData/DockerDesktop", Review, "docker system prune -a"},
		{"Загрузки драйверов NVIDIA",
			"ProgramData/NVIDIA Corporation/Downloader",
			Safe,
			"Удалить содержимое"},
		{"Установщики NVIDIA",
			"Program Files/NVIDIA Corporation/Installer2",
			Safe,
			"Удалить содержимое"},
		{"Библиотека Steam",
			"Program Files (x86)/Steam/steamapps",
			Review,
			"Удалить ненужные игры через Steam или перенести библиотеку"},
		{"Кэш шейдеров Steam", "Program Files (x86)/Steam/steamapps/shadercache", Safe, "Удалить"},
	};
}

std::vector<CleanupRule> BuildUserCacheRules()
{
	return {
		{"Временные файлы пользователя",
			"Users/*/AppData/Local/Temp",
			Safe,
			"Очистить содержимое (%TEMP%)"},
		{"Дампы падений программ", "Users/*/AppData/Local/CrashDumps", Safe, "Удалить"},
		{"Кэш Internet Explorer/WebView",
			"Users/*/AppData/Local/Microsoft/Windows/INetCache",
			Safe,
			"Удалить"},
		{"Кэш эскизов",
			"Users/*/AppData/Local/Microsoft/Windows/Explorer",
			SystemTool,
			"Очистка диска → «Эскизы»"},
		{"Кэш DirectX шейдеров", "Users/*/AppData/Local/D3DSCache", Safe, "Удалить"},
		{"Кэш шейдеров NVIDIA", "Users/*/AppData/Local/NVIDIA/DXCache", Safe, "Удалить"},
		{"Кэш шейдеров NVIDIA", "Users/*/AppData/Local/NVIDIA/GLCache", Safe, "Удалить"},
		{"Кэш шейдеров NVIDIA",
			"Users/*/AppData/LocalLow/NVIDIA/PerDriverVersion/DXCache",
			Safe,
			"Удалить"},
		{"Кэш шейдеров AMD", "Users/*/AppData/Local/AMD/DxCache", Safe, "Удалить"},
	};
}

std::vector<CleanupRule> BuildBrowserRules()
{
	return {
		{"Кэш Chrome",
			"Users/*/AppData/Local/Google/Chrome/User Data/*/Cache",
			Safe,
			"Настройки → Удалить данные → Кэш"},
		{"Кэш Chrome",
			"Users/*/AppData/Local/Google/Chrome/User Data/*/Code Cache",
			Safe,
			"Удалить при закрытом браузере"},
		{"Кэш Chrome (Service Worker)",
			"Users/*/AppData/Local/Google/Chrome/User Data/*/Service Worker/CacheStorage",
			Safe,
			"Удалить при закрытом браузере"},
		{"Кэш Edge",
			"Users/*/AppData/Local/Microsoft/Edge/User Data/*/Cache",
			Safe,
			"Настройки → Удалить данные → Кэш"},
		{"Кэш Edge",
			"Users/*/AppData/Local/Microsoft/Edge/User Data/*/Code Cache",
			Safe,
			"Удалить при закрытом браузере"},
		{"Кэш Edge (Service Worker)",
			"Users/*/AppData/Local/Microsoft/Edge/User Data/*/Service Worker/CacheStorage",
			Safe,
			"Удалить при закрытом браузере"},
		{"Кэш Яндекс Браузера",
			"Users/*/AppData/Local/Yandex/YandexBrowser/User Data/*/Cache",
			Safe,
			"Удалить при закрытом браузере"},
		{"Кэш Opera", "Users/*/AppData/Local/Opera Software/*/Cache", Safe, "Удалить"},
		{"Кэш Firefox",
			"Users/*/AppData/Local/Mozilla/Firefox/Profiles",
			Safe,
			"В Local хранится только кэш — можно удалить при закрытом браузере"},
	};
}

std::vector<CleanupRule> BuildMessengerRules()
{
	return {
		{"Кэш Discord", "Users/*/AppData/Roaming/discord/Cache", Safe, "Удалить"},
		{"Кэш Discord", "Users/*/AppData/Roaming/discord/Code Cache", Safe, "Удалить"},
		{"Кэш Telegram",
			"Users/*/AppData/Roaming/Telegram Desktop/tdata/user_data",
			Safe,
			"Настройки → Продвинутые → Управление памятью"},
		{"Кэш Teams",
			"Users/*/AppData/Local/Packages/MSTeams_8wekyb3d8bbwe/LocalCache",
			Safe,
			"Удалить при закрытом Teams"},
		{"Кэш Teams (classic)",
			"Users/*/AppData/Roaming/Microsoft/Teams",
			Review,
			"Кэш можно удалить при закрытом Teams"},
		{"Кэш Spotify", "Users/*/AppData/Local/Spotify/Storage", Safe, "Удалить"},
		{"Кэш Spotify (Store)",
			"Users/*/AppData/Local/Packages/SpotifyAB.SpotifyMusic_zpdnekdrzrea0/LocalCache",
			Safe,
			"Удалить"},
	};
}

std::vector<CleanupRule> BuildDeveloperRules()
{
	return {
		{"Кэш npm", "Users/*/AppData/Local/npm-cache", Safe, "npm cache clean --force"},
		{"Кэш Yarn", "Users/*/AppData/Local/Yarn/Cache", Safe, "yarn cache clean"},
		{"Хранилище pnpm", "Users/*/AppData/Local/pnpm/store", Safe, "pnpm store prune"},
		{"Кэш pip", "Users/*/AppData/Local/pip/Cache", Safe, "pip cache purge"},
		{"Кэш uv", "Users/*/AppData/Local/uv/cache", Safe, "uv cache clean"},
		{"Пакеты NuGet", "Users/*/.nuget/packages", Safe, "dotnet nuget locals all --clear"},
		{"Кэш Gradle", "Users/*/.gradle/caches", Safe, "Удалить при закрытой IDE"},
		{"Репозиторий Maven", "Users/*/.m2/repository", Safe, "Удалить, скачается заново"},
		{"Реестр Cargo", "Users/*/.cargo/registry", Safe, "cargo cache -a (cargo-cache)"},
		{"Модули Go", "Users/*/go/pkg/mod", Safe, "go clean -modcache"},
		{"Кэш Conan", "Users/*/.conan2/p", Safe, "conan remove \"*\" -c"},
		{"Архивы vcpkg", "Users/*/AppData/Local/vcpkg/archives", Safe, "Удалить"},
		{"Пакеты Conda", "Users/*/anaconda3/pkgs", Safe, "conda clean --all"},
		{"Пакеты Conda", "Users/*/miniconda3/pkgs", Safe, "conda clean --all"},
		{"Android SDK",
			"Users/*/AppData/Local/Android/Sdk",
			Review,
			"Удалить старые system-images через SDK Manager"},
		{"Эмуляторы Android", "Users/*/.android/avd", Review, "Удалить ненужные AVD"},
		{"Кэши JetBrains",
			"Users/*/AppData/Local/JetBrains",
			Review,
			"Старые версии IDE — удалить папки старых версий"},
		{"Кэш Visual Studio",
			"Users/*/AppData/Local/Microsoft/VisualStudio",
			Review,
			"Папки старых версий VS можно удалить"},
		{"Кэш VS Code", "Users/*/AppData/Roaming/Code/Cache", Safe, "Удалить"},
		{"Кэш VS Code", "Users/*/AppData/Roaming/Code/CachedData", Safe, "Удалить"},
		{"Хранилище воркспейсов VS Code",
			"Users/*/AppData/Roaming/Code/User/workspaceStorage",
			Safe,
			"Удалить старые воркспейсы"},
		{"Расширения VS Code", "Users/*/.vscode/extensions", Review, "Удалить неиспользуемые"},
		{"Docker Desktop (WSL-диск)",
			"Users/*/AppData/Local/Docker/wsl",
			Review,
			"docker system prune -a, затем сжать vhdx"},
		{"Данные приложений Store и WSL",
			"Users/*/AppData/Local/Packages",
			Review,
			"Здесь лежат ext4.vhdx дистрибутивов WSL — см. список крупных файлов"},
	};
}

std::vector<CleanupRule> BuildPersonalRules()
{
	return {
		{"Загрузки", "Users/*/Downloads", Review, "Разобрать: установщики, архивы, ISO"},
		{"Рабочий стол", "Users/*/Desktop", Review, "Перенести на другой диск"},
		{"Документы", "Users/*/Documents", Review, "Перенести на другой диск"},
		{"Видео", "Users/*/Videos", Review, "Перенести на другой диск"},
		{"Изображения", "Users/*/Pictures", Review, "Перенести на другой диск"},
		{"Музыка", "Users/*/Music", Review, "Перенести на другой диск"},
		{"OneDrive (локальные копии)",
			"Users/*/OneDrive",
			Review,
			"ПКМ → «Освободить место» для облачных файлов"},
	};
}

void Append(std::vector<CleanupRule>& target, std::vector<CleanupRule> source)
{
	target.insert(target.end(),
		std::make_move_iterator(source.begin()),
		std::make_move_iterator(source.end()));
}
} // namespace

std::vector<CleanupRule> BuildCleanupCatalog()
{
	std::vector<CleanupRule> catalog;

	Append(catalog, BuildSystemFileRules());
	Append(catalog, BuildWindowsRules());
	Append(catalog, BuildProgramDataRules());
	Append(catalog, BuildUserCacheRules());
	Append(catalog, BuildBrowserRules());
	Append(catalog, BuildMessengerRules());
	Append(catalog, BuildDeveloperRules());
	Append(catalog, BuildPersonalRules());

	return catalog;
}
