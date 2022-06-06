#ifndef __backbufferheader__
#define __backbufferheader__

#include <windows.h>

class backbuffer
{
	private:
	HDC bufferhdc;
	HBITMAP buffer;
	int w, h;
	
	public:
	backbuffer(HWND hwnd, int w, int h);
	~backbuffer();
	void draw_picture(HBITMAP pic, HBITMAP mask, int x, int y, int width, int height);
	void draw_picture(HBITMAP pic, int x, int y, int width, int height);
	void print(HWND hwnd);
	void clear();
	void draw_line (int x0, int y0, int xf, int yf);
	void draw_text (char *text, int x0, int y0, int xf, int yf);
};

#endif
