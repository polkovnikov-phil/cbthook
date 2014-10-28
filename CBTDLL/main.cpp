#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "shared.h"

#include <boost/scope_exit.hpp>

#include <iostream>
#include <fstream>
using namespace std;

#pragma data_seg("cbtdll")
HWND hWnd = nullptr;
DWORD pid = 0;
#pragma data_seg()

HINSTANCE hInst = nullptr;
HHOOK hHook = nullptr;

LRESULT CALLBACK cbtProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code > 0) {
		HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid);
		BOOST_SCOPE_EXIT_ALL(hProc) { CloseHandle(hProc); };
		void* holder = VirtualAllocEx(hProc, nullptr, sizeof(callback_data), MEM_COMMIT, PAGE_READWRITE);
		callback_data data = { code, wParam, lParam };
		WriteProcessMemory(hProc, holder, &data, sizeof(callback_data), nullptr);
		SendMessage(hWnd, WM_USER, reinterpret_cast<WPARAM>(holder), 0);
	}
	return CallNextHookEx(hHook, code, wParam, lParam);
}

DLLHOOKPREF bool InstallHook(HWND _hWnd) {
	if (hHook) return false;
	hWnd = _hWnd;
	GetWindowThreadProcessId(hWnd, &pid);
	return (hHook = SetWindowsHookEx(WH_CBT, cbtProc, hInst, 0)) != NULL;
}

DLLHOOKPREF bool UninstallHook() {
	if (!hHook) return false;
	bool res = UnhookWindowsHookEx(hHook) != NULL;
	if (res) hHook = NULL;
	return res;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	hInst = reinterpret_cast<HINSTANCE>(hModule);
	return TRUE;
}