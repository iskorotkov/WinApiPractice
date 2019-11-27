#pragma once
#include "SharedStorage.h"
#include "files.h"
#include <memory>

struct Preferences;
class GraphicsThread;
class GameState;
class GameRules;
class MultiplayerLauncher;

class GameSession
{
public:
	explicit GameSession(int argc, char** argv);

	GameSession(const GameSession& other) = delete;
	GameSession(GameSession&& other) noexcept = delete;
	GameSession& operator=(const GameSession& other) = delete;
	GameSession& operator=(GameSession&& other) noexcept = delete;

	void Start(HWND window);
	void GameWon() const;
	void GameLost() const;
	void GameCrashed(const std::wstring& errorMessage) const;

	[[nodiscard]] GameState* GetState() const;
	[[nodiscard]] Preferences* GetPreferences() const;
	[[nodiscard]] GraphicsThread* GetGraphicsThread() const;
	[[nodiscard]] GameRules* GetRules() const;
	[[nodiscard]] MultiplayerLauncher* GetMultiplayer() const;
	
	[[nodiscard]] bool IsStarted() const;

	~GameSession();

private:
	SharedStorage storage;
	ReadingMethod method;
	std::unique_ptr<GameState> state;
	std::unique_ptr<Preferences> preferences;
	std::unique_ptr<GraphicsThread> graphicsThread;
	std::unique_ptr<GameRules> rules;
	std::unique_ptr<MultiplayerLauncher> multiplayer;

	HWND window;
	bool isStarted = false;

	static bool IsClient(int argc, char** argv);
	void CreateRules(int argc, char** argv);
};
