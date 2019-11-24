#include "GameState.h"

GameState::GameState(std::byte* data, const int dimension) :
	dimension(dimension)
{
	values = std::vector<int>(reinterpret_cast<int*>(data), reinterpret_cast<int*>(data) + dimension * dimension);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void GameState::SetAt(const int line, const int column, const int value)
{
	values[Index(line, column)] = value;
}

int GameState::GetAt(const int line, const int column) const
{
	return values[Index(line, column)];
}

int GameState::Index(const int line, const int column) const
{
	return line * dimension + column;
}
