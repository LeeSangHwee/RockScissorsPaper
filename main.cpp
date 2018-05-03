#include "MyImage.h"
#include "dsSpriteManager.h"
#include "dsOpenALSoundManager.h"

Graphics *g_BackBuffer;
Graphics *g_MainBuffer;
Bitmap   *g_Bitmap;

int g_rselect = 0;
int g_lselect = 0;
bool g_isSelect = false;
int g_lWin = 0;
int g_rWin = 0;
int state = 1;
int tmp = -1;

dsSprite *g_pSprite[3];
dsSprite *g_pNumber[10];
dsSprite *g_pReady;
dsSprite *g_pWin;
dsSprite *g_pLose;

void OnUpdate(HWND hWnd, DWORD tick);
void CreateBuffer(HWND hWnd, HDC hDC);
void ReleaseBuffer(HWND hWnd, HDC hDC);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,     int nCmdShow)
{
	WNDCLASS   wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "HelloWin";
	
	if(RegisterClass(&wndclass) == 0)
	{
		return 0;		
	}

	HWND hwnd = CreateWindow("HelloWin", "Rock Scissors Paper", 
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
                              NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		return 0;
	}

	HDC hDC = GetDC(hwnd);

	CreateBuffer(hwnd, hDC);

	dsSpriteManager *pSpriteManager = GetSpriteManager();

	g_pReady = pSpriteManager->Create("ready", "ready.png");

	g_pWin = pSpriteManager->Create("win", "win.png");
	g_pLose = pSpriteManager->Create("lose", "lose.png");

	g_pSprite[0] = pSpriteManager->Create("scissors", "scissors.jpg");
	g_pSprite[1] = pSpriteManager->Create("rock", "rock.jpg");
	g_pSprite[2] = pSpriteManager->Create("paper", "paper.jpg");
	
	char name[128];
	for (int i = 0; i < 10; i++)
	{
		wsprintf(name, "g_%02d.png", i);
		g_pNumber[i] = pSpriteManager->Create(name, name);
	}

	dsOpenALSoundManager *pSoundManger = GetOpenALSoundManager();
	dsSound *pSound = pSoundManger->LoadSound("test.wav", true);
	if (pSound)
	{
		//pSound->Play();
	}

	ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	MSG msg;
	DWORD tick = GetTickCount();
	while(1)
	{	
		//������ �޼����� ������� �޼����� ó���Ѵ�.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //�޼����� ���� ��� ���� ������ �����Ѵ�.
		{		
			DWORD curTick = GetTickCount();
			OnUpdate(hwnd, curTick - tick);
			tick = curTick;						
			
			g_MainBuffer->DrawImage(g_Bitmap, 0, 0);			
		}
	}

	ReleaseBuffer(hwnd, hDC);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_BACK: // ���� �ٽ� ����
				g_rselect = 0;
				g_lselect = 0;
				g_isSelect = false;
				g_lWin = 0;
				g_rWin = 0;
				state = 1;
				tmp = -1;
				break;
			case VK_SPACE: // ���� ��� �ϱ�
				g_isSelect = false;			
				tmp = -1;
				break;
			case VK_NUMPAD1: // ����
				g_isSelect = true;
				g_lselect = 0;
				g_rselect = rand() % 3;
				break;
			case VK_NUMPAD2: // ����
				g_isSelect = true;
				g_lselect = 1;
				g_rselect = rand() % 3;
				break;
			case VK_NUMPAD3: // ��
				g_isSelect = true;
				g_lselect = 2;
				g_rselect = rand() % 3;
				break;
			}	

			// ���������� ��
			if (g_isSelect)
			{
				tmp = g_lselect - g_rselect;
				if (tmp < 0)
				{
					tmp += 3;
				}
				switch (tmp)
				{
				case 0: break;
				case 1: g_lWin++; break;
				case 2: g_rWin++; break;
				}
			}
		}
		break;
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateBuffer(HWND hWnd, HDC hDC)
{
	GdiplusStartupInput			gdiplusStartupInput;
	ULONG_PTR					gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	RECT rc;
	GetClientRect(hWnd, &rc);

	g_Bitmap = new Bitmap(rc.right - rc.left, rc.bottom - rc.top);
	g_BackBuffer = new Graphics(g_Bitmap);
	g_BackBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);

	g_MainBuffer = new Graphics(hDC);
	g_MainBuffer->SetPageUnit(Gdiplus::Unit::UnitPixel);
}

void ReleaseBuffer(HWND hWnd, HDC hDC)
{
	ReleaseDC(hWnd, hDC);

	delete g_Bitmap;
	delete g_BackBuffer;
	delete g_MainBuffer;
}

void OnUpdate(HWND hWnd, DWORD tick)
{
	if (hWnd == NULL)
		return;
	
	Color color(255, 255, 255);
	
	int v1, v2, xpos;
	float StartCount = timeGetTime() * 0.001f; // ���� �ð�
	float EndCount, Sec;

	// ���� ���� ��
	switch (state)
	{
	case 0: // ������ �Է� ���� �� �ʱ�ȭ
		state = 2;
		g_isSelect = false;
		g_rWin = 0;
		g_lWin = 0;
		break;
	case 1: // ���� ���� ��
		while (state)
		{
			g_BackBuffer->Clear(color); // ��� ����
			g_pReady->Draw(g_BackBuffer, 200, 200, 504, 170); // Ready ���
			
			EndCount = timeGetTime() * 0.001f; // ������ �ð�
			Sec = (EndCount - StartCount) * 0.8f; // �ð� ���
						
			if (1.0f > Sec) // ī��Ʈ 3 ���
				g_pNumber[3]->Draw(g_BackBuffer, 375, 380);
			else if (2.0f > Sec) // ī��Ʈ 2 ���
				g_pNumber[2]->Draw(g_BackBuffer, 375, 380);
			else if (3.0f > Sec) // ī��Ʈ 1 ���
				g_pNumber[1]->Draw(g_BackBuffer, 375, 380);
			else if (Sec >= 3.0f) // 3�� ��� �� ���°� 0
				state = 0;

			g_MainBuffer->DrawImage(g_Bitmap, 0, 0);
		}
		break;
	case 2: // ���� ����
		g_BackBuffer->Clear(color);

		// ���� ����
		v1 = g_rWin / 10;
		v2 = g_rWin % 10;
		xpos = 150;

		if (v1 > 0)
		{
			g_pNumber[v1]->Draw(g_BackBuffer, xpos - 55, 150);
			xpos += 55;
		}
		g_pNumber[v2]->Draw(g_BackBuffer, xpos, 150);

		// ������ ����
		v1 = g_lWin / 10;
		v2 = g_lWin % 10;
		xpos = 550;

		if (v1 > 0)
		{
			g_pNumber[v1]->Draw(g_BackBuffer, xpos - 55, 150);
			xpos += 55;
		}
		g_pNumber[v2]->Draw(g_BackBuffer, xpos, 150);


		// ���������� ���
		if (g_isSelect) // ������ ���������� ���
		{
			g_pSprite[g_rselect]->Draw(g_BackBuffer, 100, 300, 250, 250);
			g_pSprite[g_lselect]->Draw(g_BackBuffer, 500, 300, 250, 250);
		}
		else // ���� �� ���� ���
		{
			g_rselect %= 3;
			g_pSprite[g_rselect++]->Draw(g_BackBuffer, 100, 300, 250, 250);

			g_lselect %= 3;
			g_pSprite[g_lselect++]->Draw(g_BackBuffer, 500, 300, 250, 250);
		}
		break;
	}

	// �¸� �й� ���
	switch (tmp)
	{
	case 0: // ���º� �� ���� ���� ���
		g_pLose->Draw(g_BackBuffer, 150, 560);
		g_pLose->Draw(g_BackBuffer, 560, 560);
		break;
	case 1: // ���� �¸�
		g_pWin->Draw(g_BackBuffer, 560, 560);
		g_pLose->Draw(g_BackBuffer, 150, 560);
		break;
	case 2: // ���� �й�
		g_pWin->Draw(g_BackBuffer, 150, 560);
		g_pLose->Draw(g_BackBuffer, 560, 560);
		break;
	}
}