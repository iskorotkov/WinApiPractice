#include "GameSession.h"
#include "Windows.h"
#include "files.h"
#include "commandline.h"
#include <iostream>
#include <memory>
#include "preferences.h"
#include "GraphicsThread.h"
#include "GameState.h"
#include "GameRules.h"
#include "MultiplayerLauncher.h"

bool GameSession::IsClient(const int argc, char** argv)
{
	if (argc >= 3)
	{
		const std::string appType(argv[2]);
		if (appType == "-client")
		{
			return true;
		}
	}
	return false;
}

void GameSession::CreateRules(const int argc, char** argv)
{
	const auto sign = IsClient(argc, argv) ? 2 : 1;
	rules = std::make_unique<GameRules>(this, sign);
}

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
	CreateRules(argc, argv);

	try
	{
		multiplayer = std::make_unique<MultiplayerLauncher>();
		if (!IsClient(argc, argv))
		{
			// TODO: Maybe we can analyze whether the instance is client only one time and save the result?
			multiplayer->LaunchClient();
		}
	}
	catch (std::exception& e)
	{
		const std::string s(e.what());
		const std::wstring ws(s.cbegin(), s.cend());
		GameError(ws);
	}
}

void GameSession::Start(HWND window)
{
	// TODO: Double initialization.
	graphicsThread = std::make_unique<GraphicsThread>(window, this);
	graphicsThread->Launch();

	isStarted = true;
}

void GameSession::GameWon() const
{
	MessageBox(nullptr, L"You have WON the game", L"Win!", MB_OK | MB_ICONASTERISK);
	PostQuitMessage(0);
}

void GameSession::GameLost() const
{
	MessageBox(nullptr, L"You have LOST the game", L"Lose!", MB_OK | MB_ICONASTERISK);
	PostQuitMessage(0);
}

void GameSession::GameError(const std::wstring& errorMessage) const
{
	MessageBox(nullptr, errorMessage.c_str(), L"Error occured!", MB_OK | MB_ICONERROR);
	PostQuitMessage(0);
}

void GameSession::PlayerMistake(const std::wstring& mistakeMessage) const
{
	MessageBox(nullptr, mistakeMessage.c_str(), L"Error occured!", MB_OK | MB_ICONWARNING);
}

GameState* GameSession::GetState() const
{
	return state.get();
}

Preferences* GameSession::GetPreferences() const
{
	return preferences.get();
}

GraphicsThread* GameSession::GetGraphicsThread() const
{
	return graphicsThread.get();
}

GameRules* GameSession::GetRules() const
{
	return rules.get();
}

MultiplayerLauncher* GameSession::GetMultiplayer() const
{
	return multiplayer.get();
}

bool GameSession::IsStarted() const
{
	return isStarted;
}

GameSession::~GameSession()
{
	WriteConfigFile(method, preferences.get());
}
