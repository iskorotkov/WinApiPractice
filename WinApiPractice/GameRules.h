#pragma once
#include "Windows.h"

class GameSession;

class GameRules
{
public:
	explicit GameRules(GameSession* game);

	class Status
	{
	public:
		static Status Win();
		static Status Lose();
		static Status InProgress();

		[[nodiscard]] bool HasWon() const { return won; }
		[[nodiscard]] bool HasLost() const { return lost; }
		[[nodiscard]] bool IsInProgress() const { return !won && !lost; }

	protected:
		Status(const bool won, const bool lost) : won(won), lost(lost)
		{
		}

	private:
		bool won = false;
		bool lost = false;
	};

	void RespondToTurnMessage(WPARAM wParam, LPARAM lParam);
	void SetupGame();
	void TurnMade();
	void StartOutTurn();

	[[nodiscard]] Status GetStatus() const;
	[[nodiscard]] bool IsOurTurn() const;

private:
	bool isOurTurn = false;
	GameSession* game;

	// TODO: How to determine whether it's our sign or not?
	int ourSign;

	void NotifyOthersAboutTurn();

	[[nodiscard]] bool IsOurSign(int value) const;
	[[nodiscard]] bool IsOpponentSign(int value) const;
};
