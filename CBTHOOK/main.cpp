#include <iostream>
#include <locale>

#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "../cbtdll/shared.h"
using namespace std;

int main() {
	_setmode(_fileno(stdout), _O_U8TEXT);
	WNDCLASS windowClass = {};
	windowClass.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT {
		if (message == WM_DESTROY) {
			UninstallHook();
		} else if (message == WM_USER) {
			callback_data* data = reinterpret_cast<callback_data*>(wParam);
			switch (data->code) {
				case HCBT_CREATEWND:
					wcout << (HWND) data->wParam << endl;
					break;
			}
			if (!VirtualFree(data, sizeof(callback_data), MEM_DECOMMIT))
				throw bad_alloc();
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	};
	LPCWSTR windowClassName = L"Foobar";
	windowClass.lpszClassName = windowClassName;
	if (!RegisterClass(&windowClass)) {
		wcerr << L"Failed to register window class" << endl;
		return 1;
	}
	HWND messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
	if (!messageWindow) {
		wcerr << L"Failed to create message-only window" << endl;
		return 1;
	}

	InstallHook(messageWindow);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
