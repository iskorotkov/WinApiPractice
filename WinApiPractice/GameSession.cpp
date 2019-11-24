#include "GameSession.h"
#include "Windows.h"
#include "files.h"
#include "commandline.h"
#include <iostream>
#include <memory>
#include "preferences.h"

GameSession::GameSession(const int argc, char** argv)
{
	method = GetReadingMethod(argc, argv);
	preferences.reset(ReadConfigFile(method));
	if (!preferences)
	{
		MessageBox(nullptr, L"Failed to read config file!", L"Error", MB_OK | MB_ICONERROR);
		throw std::exception("Failed to read config file!");
	}

	const auto dimension = preferences->GridSize;
	try
	{
		const auto len = dimension * dimension;
		storage.Open(L"GameGrid", len * sizeof(int));
	}
	catch (std::exception& e)
	{
		std::cout << "An error happened during shared storage opening. " << '\n' << e.what();
		throw;
	}

	state = std::make_unique<GameState>(storage.GetStorage(), dimension);
}

GameState* GameSession::GetGameState() const
{
	return state.get();
}

Preferences* GameSession::GetPreferences() const
{
	return preferences.get();
}

GameSession::~GameSession()
{
	WriteConfigFile(method, preferences.get());
}
