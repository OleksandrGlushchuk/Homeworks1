#pragma once
#include <Windows.h>
class InputState
{
public:
	bool KEY_W=0, KEY_S=0, KEY_A=0, KEY_D=0, KEY_Q=0, KEY_E=0, KEY_SPACE=0, KEY_CONTROL=0, KEY_NULL=0;
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
		if (key == VK_SPACE)
			return KEY_SPACE;
		if (key == VK_CONTROL)
			return KEY_CONTROL;
		return KEY_NULL;
	}
};