#include <windows.h>
#include "backbuffer.h"

backbuffer::backbuffer(HWND hwnd, int w, int h)
{
	HDC mainhdc = GetDC(hwnd);
	this->bufferhdc = CreateCompatibleDC(mainhdc);
	this->buffer = CreateCompatibleBitmap(mainhdc, w, h);
	SelectObject(this->bufferhdc, this->buffer);
	this->w = w;
	this->h = h;
	ReleaseDC(hwnd, mainhdc);
}

backbuffer::~backbuffer ()
{
	DeleteObject(this->buffer);
	DeleteDC(this->bufferhdc);
}

void backbuffer::draw_picture (HBITMAP pic, HBITMAP mask, int x, int y, int width, int height)
{
	HDC hdcMemory;
	hdcMemory = CreateCompatibleDC(this->bufferhdc);
   
	SelectObject(hdcMemory, mask);
	BitBlt(this->bufferhdc, x, y, width, height, hdcMemory, 0, 0, SRCAND);

	SelectObject(hdcMemory, pic);
	BitBlt(this->bufferhdc, x, y, width, height, hdcMemory, 0, 0, SRCPAINT);
   
	DeleteDC(hdcMemory);
}

void backbuffer::draw_picture (HBITMAP pic, int x, int y, int width, int height)
{
	HDC hdcMemory;
	hdcMemory = CreateCompatibleDC(this->bufferhdc);

	SelectObject(hdcMemory, pic);
	BitBlt(this->bufferhdc, x, y, width, height, hdcMemory, 0, 0, SRCCOPY);
   
	DeleteDC(hdcMemory);
}

void backbuffer::print(HWND hwnd)
{
	HDC mainhdc = GetDC(hwnd);
	BitBlt(mainhdc, 0, 0, this->w, this->h, this->bufferhdc, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, mainhdc);
}

void backbuffer::clear ()
{
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = this->w;
	rc.bottom = this->h;
	FillRect(this->bufferhdc, &rc, (HBRUSH)(COLOR_BTNFACE+1));
}

void backbuffer::draw_line (int x0, int y0, int xf, int yf)
{
	MoveToEx(this->bufferhdc, x0, y0, NULL);
	LineTo(this->bufferhdc, xf, yf);
}

void backbuffer::draw_text (char *text, int x0, int y0, int xf, int yf)
{
	RECT rc;
	rc.left = x0;
	rc.top = y0;
	rc.right = xf;
	rc.bottom = yf;
	
	DrawText (this->bufferhdc, text, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}
