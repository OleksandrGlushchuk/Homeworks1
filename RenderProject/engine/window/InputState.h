#pragma once
#include <Windows.h>
class InputState
{
public:
	bool KEY_W = 0, KEY_S = 0, KEY_A = 0, KEY_D = 0, KEY_Q = 0, KEY_E = 0, KEY_SPACE = 0, KEY_CONTROL = 0, KEY_NULL = 0,
		KEY_PLUS = 0, KEY_MINUS = 0, KEY_R = 0, KEY_SHIFT = 0, KEY_1 = 0, KEY_2 = 0, KEY_3 = 0, KEY_4 = 0;
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
		if (key == VK_SHIFT)
			return KEY_SHIFT;
		if (key == VK_SPACE)
			return KEY_SPACE;
		if (key == VK_CONTROL)
			return KEY_CONTROL;
		if (key == VK_OEM_PLUS)
			return KEY_PLUS;
		if (key == VK_OEM_MINUS)
			return KEY_MINUS;
		if (key == 'R')
			return KEY_R;
		if (key == '1')
			return KEY_1;
		if (key == '2')
			return KEY_2;
		if (key == '3')
			return KEY_3;
		if (key == '4')
			return KEY_4;
		return KEY_NULL;
	}
};