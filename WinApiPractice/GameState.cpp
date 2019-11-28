#include "GameState.h"

GameState::GameState(std::byte* data, const int dimension) :
	values(reinterpret_cast<int*>(data)),
	dimension(dimension)
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
void GameState::SetAt(const int line, const int column, const int value)
{
	const auto index = Index(line, column);
	std::lock_guard lock(m);
	if (values[index] != 0)
	{
		throw std::exception("Can't add a mark here. The cell is taken.");
	}
	values[index] = value;
}

int GameState::GetDimension() const
{
	return dimension;
}

int GameState::GetAt(const int line, const int column) const
{
	const auto index = Index(line, column);
	std::lock_guard lock(m);
	return values[index];
}

int GameState::Index(const int line, const int column) const
{
	return line * dimension + column;
}
