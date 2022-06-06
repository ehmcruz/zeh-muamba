#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "config.h"
#include "snuke.h"
#include "main.h"
#include "resources.h"
#include "highscore.h"

char g_szClassName[] = "MyWindowClass";
HINSTANCE g_hInst = NULL;
snuke *game;
void *data_exchange;

#if DEBUGMOD==1
	FILE *fpdebug;
#endif

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static int mouse_x, mouse_y;
	
	switch(Message) {
		
		case WM_CREATE:
			#if DEBUGMOD==1
				fpdebug = fopen("snuke_debug.txt", "w");
				fprintf(fpdebug, "window created\n");
			#endif
			game = NULL;
			break;
           
        case WM_MOUSEMOVE:
			mouse_x = GET_X_LPARAM( lParam );
           	mouse_y = GET_Y_LPARAM( lParam );
			break;
           
		case WM_LBUTTONDOWN:
			if (game)
				game->click();
			break;
			
		case WM_KEYDOWN:
			if (game) {
				if (wParam > 0+ASCII_N_START && wParam < 10+ASCII_N_START)
					game->set_mult_factor((int)wParam - ASCII_N_START);
			}
			break;
			
		case WM_TIMER:
			game->print_rendered(hwnd, mouse_x, mouse_y);
			break;

		case WM_PAINT:   {
			HBITMAP intro;
			HDC hdc, hdcpic;/* A device context used for drawing */
			PAINTSTRUCT ps;/* Also used during window drawing */
			
			intro = LoadBitmap(g_hInst, "INTROBMP");
			if (!intro) {
    			MessageBox(hwnd, "Load of resources failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
//				return;
			}

			hdc = BeginPaint (hwnd, &ps);
			hdcpic = CreateCompatibleDC(hdc);
			SelectObject(hdcpic, intro);
			BitBlt(hdc, 0, 0, TABLE_XSIZE, TABLE_YSIZE, hdcpic, 0, 0, SRCCOPY);
			DeleteDC(hdcpic);
			
			EndPaint (hwnd, &ps);
			DeleteObject(intro);
			break;
		}
		
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			
				case CM_NEWGAME:
					mouse_x = mouse_y = 10;
					if (game) {
						#if DEBUGMOD==1
							fprintf(fpdebug, "game restarted\n");
						#endif
						KillTimer(hwnd, TEMPID);
						delete game;
					}
					#if DEBUGMOD==1
						else
							fprintf(fpdebug, "game started\n");
					#endif
					game = new snuke(hwnd, SCREEN_XSIZE, SCREEN_YSIZE, NEWGAME); 
					game->render_next_frame(hwnd, mouse_x, mouse_y);
					SetTimer(hwnd, TEMPID, TEMPDELAY, NULL);
					break;
					
				case CM_ABOUT:
                     if (game) {
                        //game->pause();
                        KillTimer(hwnd, TEMPID);
                        DialogBox(g_hInst, (LPSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)AboutDlgProc);
                        SetTimer(hwnd, TEMPID, TEMPDELAY, NULL);
                        //game->unpause();
                     }
                     else
                        DialogBox(g_hInst, (LPSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)AboutDlgProc);
                     break;
                     
				case CM_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
			
		case WM_DESTROY:
			#if DEBUGMOD==1
				fprintf(fpdebug, "game exit\n");
				fclose(fpdebug);
			#endif
			KillTimer(hwnd, TEMPID);
			if (game)
				delete game;
			PostQuitMessage(0);
			break;
			
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

///////////////////////

LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_INITDIALOG:
         break;

      case WM_COMMAND:
         if (LOWORD(wParam) == IDOK) 
         {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
         }
         break;
   }
   return FALSE;
}

///////////////////////

LRESULT CALLBACK ScoreDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_INITDIALOG:
         break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK: {
                    struct itemhs hs;
                    int *pscore;
                    GetDlgItemText(hDlg, Edit1, hs.name, 30);
                    pscore = (int *)data_exchange;
                    hs.score = *pscore;
                    game->hs->insert(&hs);
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return FALSE;
         }
         break;
   }
   return FALSE;
}

///////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX WndClass;
   HWND hwnd;
   MSG Msg;

   g_hInst = hInstance;

   WndClass.cbSize        = sizeof(WNDCLASSEX);
   WndClass.style         = 0;
   WndClass.lpfnWndProc   = WndProc;
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = g_hInst;
   WndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
   WndClass.lpszMenuName  = "MAINMENU";
   WndClass.lpszClassName = g_szClassName;
   WndClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

   if(!RegisterClassEx(&WndClass))
   {
      MessageBox(0, "Window Registration Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   hwnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      g_szClassName,
      "Zeh Muamba Snuke Gamer",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_XSIZE, SCREEN_YSIZE+50,
      NULL, NULL, g_hInst, NULL);

   if(hwnd == NULL)
   {
      MessageBox(0, "Window Creation Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while(GetMessage(&Msg, NULL, 0, 0))
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return Msg.wParam;
}
