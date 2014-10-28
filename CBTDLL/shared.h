#pragma once
#include <windows.h>

#ifdef CBTDLL_EXPORTS
#define DLLHOOKPREF __declspec(dllexport)
#else
#define DLLHOOKPREF __declspec(dllimport)
#endif
typedef void(*DECODERPROC)(int code, WPARAM wParam, LPARAM lParam);

struct callback_data {
	int code;
	WPARAM wParam;
	LPARAM lParam;
};

DLLHOOKPREF bool InstallHook(HWND _hWnd);
DLLHOOKPREF bool UninstallHook();

#ifndef CBTDLL_EXPORTS
#undef DLLHOOKPREF
#endif