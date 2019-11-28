#pragma once
#include <mutex>

class GameState
{
public:
	explicit GameState(std::byte* data, int dimension);

	void SetAt(int line, int column, int value);

	[[nodiscard]] int GetDimension() const;
	[[nodiscard]] int GetAt(int line, int column) const;

private:
	int* values;
	int dimension;
	mutable std::mutex m;

	[[nodiscard]] int Index(int line, int column) const;
};

