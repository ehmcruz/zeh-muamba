#ifndef __mainheader__
#define __mainheader__

#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ScoreDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif
