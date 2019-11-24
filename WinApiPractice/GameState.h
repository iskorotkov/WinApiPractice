#pragma once
#include "memory"

class GameState
{
public:
	explicit GameState(std::byte* data, int dimension);

	void SetAt(int line, int column, int value);

	[[nodiscard]] int GetAt(int line, int column) const;

private:
	int* values;
	int dimension;

	[[nodiscard]] int Index(int line, int column) const;
};

