#include "GameRules.h"
#include <stdexcept>
#include "GameSession.h"
#include "GameState.h"

GameRules::GameRules(GameSession* game) : game(game)
{
}

GameRules::Status GameRules::Status::Win()
{
	return { true, false };
}

GameRules::Status GameRules::Status::Lose()
{
	return { false, true };
}

GameRules::Status GameRules::Status::InProgress()
{
	return { false, false };
}

void GameRules::RespondToTurnMessage(WPARAM wParam, LPARAM lParam)
{
	// Check if it's our turn
}

void GameRules::SetupGame()
{
	// Register message
}

void GameRules::TurnMade()
{
	if (!IsOurTurn())
	{
		throw std::logic_error("Can't make turn if its opponents turn now.");
	}
	isOurTurn = false;
	NotifyOthersAboutTurn();
}

void GameRules::StartOutTurn()
{
	if (IsOurTurn())
	{
		throw std::logic_error("Can't start our turn because it's already started.");
	}
	isOurTurn = true;
}

GameRules::Status GameRules::GetStatus() const
{
	const auto state = game->GetGameState();
	const auto dimension = state->GetDimension();

	const auto handleResult = [dimension](auto ours, auto opponents)
	{
		if (ours == dimension)
		{
			return Status::Win();
		}
		if (opponents == dimension)
		{
			return Status::Lose();
		}
	};

	// Rows
	for (auto row = 0; row < dimension; ++row)
	{
		auto ours = 0;
		auto opponents = 0;
		for (auto column = 0; column < dimension; ++column)
		{
			if (IsOurSign(state->GetAt(row, column)))
			{
				++ours;
			}
			else if (IsOpponentSign(state->GetAt(row, column)))
			{
				++opponents;
			}
			const auto result = handleResult(ours, opponents);
			if (!result.IsInProgress())
			{
				return result;
			}
		}
	}

	// Columns
	for (auto column = 0; column < dimension; ++column)
	{
		auto ours = 0;
		auto opponents = 0;
		for (auto row = 0; row < dimension; ++row)
		{
			if (IsOurSign(state->GetAt(row, column)))
			{
				++ours;
			}
			else if (IsOpponentSign(state->GetAt(row, column)))
			{
				++opponents;
			}
			const auto result = handleResult(ours, opponents);
			if (!result.IsInProgress())
			{
				return result;
			}
		}
	}

	// Diagonals
	auto ours = false;
	auto opponents = false;
	for (auto row = 0; row < dimension; ++row)
	{
		if (IsOurSign(state->GetAt(row, row)))
		{
			ours = true;
		}
		else if (IsOpponentSign(state->GetAt(row, row)))
		{
			opponents = true;
		}
		if (ours && opponents)
		{
			return Status::InProgress();
		}
	}

	return Status::InProgress();
}

bool GameRules::IsOurTurn() const
{
	return isOurTurn;
}

void GameRules::NotifyOthersAboutTurn()
{
	// Broadcast message
}

bool GameRules::IsOurSign(const int value) const
{
	return value == ourSign;
}

bool GameRules::IsOpponentSign(const int value) const
{
	return value != ourSign;
}
