﻿#include "GameRules.h"
#include <stdexcept>
#include "GameSession.h"
#include "GameState.h"

GameRules::GameRules(GameSession* game, const int sign) : game(game), ourSign(sign)
{
	SetupGame();
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

void GameRules::RespondToTurnMessage(const WPARAM wParam, const LPARAM)
{
	const auto status = GetStatus();
	if (status.HasWon())
	{
		game->GameWon();
	}
	else if (status.HasLost())
	{
		game->GameLost();
	}

	// Check if it's our turn
	if (wParam != GetCurrentProcessId())
	{
		isOurTurn = true;
	}
}

void GameRules::SetupGame()
{
	// Register message
	turnMessageCode = RegisterWindowMessage(L"WM_TURNMADE");
}

void GameRules::FinishTurn()
{
	isOurTurn = false;
	NotifyOthersAboutTurn();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void GameRules::StartTurn()
{
	if (!IsOurTurn())
	{
		throw std::logic_error("Can't make turn.\nWait for opponent's turn.");
	}
}

GameRules::Status GameRules::GetStatus() const
{
	// TODO: Refactor large method.
	const auto state = game->GetState();
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
		return Status::InProgress();
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
		}
		const auto result = handleResult(ours, opponents);
		if (!result.IsInProgress())
		{
			return result;
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
		}
		const auto result = handleResult(ours, opponents);
		if (!result.IsInProgress())
		{
			return result;
		}
	}

	{
		// Main diagonal
		auto ours = 0;
		auto opponents = 0;
		for (auto row = 0; row < dimension; ++row)
		{
			const auto column = row;
			if (IsOurSign(state->GetAt(row, column)))
			{
				++ours;
			}
			else if (IsOpponentSign(state->GetAt(row, column)))
			{
				++opponents;
			}
			if (ours > 0 && opponents > 0)
			{
				return Status::InProgress();
			}
		}
		const auto result = handleResult(ours, opponents);
		if (!result.IsInProgress())
		{
			return result;
		}
	}

	{
		// Second diagonal
		auto ours = 0;
		auto opponents = 0;
		for (auto row = 0; row < dimension; ++row)
		{
			const auto column = dimension - row - 1;
			if (IsOurSign(state->GetAt(row, column)))
			{
				++ours;
			}
			else if (IsOpponentSign(state->GetAt(row, column)))
			{
				++opponents;
			}
			if (ours > 0 && opponents > 0)
			{
				return Status::InProgress();
			}
		}
		const auto result = handleResult(ours, opponents);
		if (!result.IsInProgress())
		{
			return result;
		}
	}
	return Status::InProgress();
}

bool GameRules::IsOurTurn() const
{
	return isOurTurn;
}

unsigned GameRules::GetTurnMessageCode() const
{
	return turnMessageCode;
}

int GameRules::GetOurSign() const
{
	// TODO: Move GetSign() method somewhere else; it's not in the right place.
	return ourSign;
}

void GameRules::NotifyOthersAboutTurn() const
{
	// Broadcast message
	// SendMessage(HWND_BROADCAST, turnMessageCode, GetCurrentProcessId(), 0);
	PostMessage(HWND_BROADCAST, turnMessageCode, GetCurrentProcessId(), 0);
}

bool GameRules::IsOurSign(const int value) const
{
	return value == ourSign;
}

bool GameRules::IsOpponentSign(const int value) const
{
	return value > 0 && value != ourSign;
}
