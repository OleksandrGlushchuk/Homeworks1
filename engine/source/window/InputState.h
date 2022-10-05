#pragma once
#include <Windows.h>
class InputState
{
public:
	bool KEY_W = 0, KEY_S = 0, KEY_A = 0, KEY_D = 0, KEY_Q = 0, KEY_E = 0, KEY_N = 0, KEY_K = 0, KEY_M = 0, KEY_J = 0, KEY_H = 0, KEY_COMMA = 0, KEY_PERIOD = 0, KEY_NULL = 0,
		 KEY_SHIFT = 0, KEY_1 = 0, KEY_2 = 0, KEY_3 = 0, KEY_4 = 0, KEY_5 = 0, KEY_UP = 0, KEY_DOWN = 0, KEY_LEFT = 0, KEY_RIGHT = 0,
		KEY_C = 0, KEY_PLUS = 0, KEY_MINUS = 0;
	bool& operator[](WPARAM key)
	{
		if (key == 'W')
			return KEY_W;
		if (key == 'S')
			return KEY_S;
		if (key == 'A')
			return KEY_A;
		if (key == 'D')
			return KEY_D;
		if (key == 'Q')
			return KEY_Q;
		if (key == 'E')
			return KEY_E;
		if (key == 'N')
			return KEY_N;
		if (key == 'K')
			return KEY_K;
		if (key == 'M')
			return KEY_M;
		if (key == 'J')
			return KEY_J;
		if (key == 'H')
			return KEY_H;
		if (key == VK_SHIFT)
			return KEY_SHIFT;
		if (key == VK_OEM_COMMA)
			return KEY_COMMA;
		if (key == VK_OEM_PERIOD)
			return KEY_PERIOD;
		if (key == VK_OEM_PLUS)
			return KEY_PLUS;
		if (key == VK_OEM_MINUS)
			return KEY_MINUS;
		if (key == '1')
			return KEY_1;
		if (key == '2')
			return KEY_2;
		if (key == '3')
			return KEY_3;
		if (key == '4')
			return KEY_4;
		if (key == '5')
			return KEY_5;
		if (key == VK_UP)
			return KEY_UP;
		if (key == VK_DOWN)
			return KEY_DOWN;
		if (key == VK_LEFT)
			return KEY_LEFT;
		if (key == VK_RIGHT)
			return KEY_RIGHT;
		if (key == 'C')
			return KEY_C;

		return KEY_NULL;
	}
};