#pragma once

#include "app/launch/LaunchOptions.hpp"

class Application
{
public:
	Application(int argc, char* argv[]);
	~Application();

	void Run() const;

private:
	LaunchOptions m_options;
};
