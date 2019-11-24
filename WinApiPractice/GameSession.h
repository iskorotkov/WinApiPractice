#pragma once
#include "GameState.h"
#include "SharedStorage.h"
#include "files.h"

struct Preferences;

class GameSession
{
public:
	explicit GameSession(int argc, char** argv);

	GameSession(const GameSession& other) = delete;
	GameSession(GameSession&& other) noexcept = delete;
	GameSession& operator=(const GameSession& other) = delete;
	GameSession& operator=(GameSession&& other) noexcept = delete;

	[[nodiscard]] GameState* GetGameState() const;
	[[nodiscard]] Preferences* GetPreferences() const;
	
	~GameSession();

private:
	SharedStorage storage;
	ReadingMethod method;
	std::unique_ptr<GameState> state;
	std::unique_ptr<Preferences> preferences;
};
