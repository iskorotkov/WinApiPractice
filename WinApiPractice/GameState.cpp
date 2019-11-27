#include "GameState.h"

GameState::GameState(std::byte* data, const int dimension) :
	values(reinterpret_cast<int*>(data)),
	dimension(dimension)
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
void GameState::SetAt(const int line, const int column, const int value)
{
	values[Index(line, column)] = value;
}

int GameState::GetDimension() const
{
	return dimension;
}

int GameState::GetAt(const int line, const int column) const
{
	return values[Index(line, column)];
}

int GameState::Index(const int line, const int column) const
{
	return line * dimension + column;
}
