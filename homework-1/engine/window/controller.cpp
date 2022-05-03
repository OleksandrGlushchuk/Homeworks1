#include "controller.h"

void Controller::OnKeyDown()
{
	if (GetKeyState('W') < 0)
	{
		render.need_to_redraw = true;
		render.sp.center[1] -= 5;
	}
	if (GetKeyState('S') < 0)
	{
		render.need_to_redraw = true;
		render.sp.center[1] += 5;
	}
	if (GetKeyState('A') < 0)
	{
		render.need_to_redraw = true;
		render.sp.center[0] -= 5;
	}
	if (GetKeyState('D') < 0)
	{
		render.need_to_redraw = true;
		render.sp.center[0] += 5;
	}
}

void Controller::OnMouseMove(WORD x, WORD y)
{
	render.sp.center[0] = x;
	render.sp.center[1] = y;
	render.need_to_redraw = true;
}

void Controller::OnChangeWindowSize(Window &window)
{
	UpdateWindowSize(window);
	render.Init(window);
	render.RedrawOn(window);
	render.DrawOn(window);
}
