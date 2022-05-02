#include "window/window.h"

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprev, _In_ LPWSTR cmdline, _In_ int cmdshow)
{
	Window window(L"homework-1", hinstance);
	window.Show();
	window.Run();
	return TRUE;
}