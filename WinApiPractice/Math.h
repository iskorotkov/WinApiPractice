#pragma once

namespace Math
{
	template <typename T, typename U>
	T Clamp(T value, U min, U max)
	{
		if (value <= min)
		{
			return min;
		}
		if (value >= max)
		{
			return max;
		}
		return value;
	}

	template <typename T>
	T Interpolate(T from, T to, float value)
	{
		value = Clamp(value, 0.0f, 1.0f);
		return from * (1 - value) + to * value;
	}

	int Floor(const float value);
}
