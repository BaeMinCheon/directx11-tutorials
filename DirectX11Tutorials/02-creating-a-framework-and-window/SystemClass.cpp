
#include "SystemClass.h"

SystemClass::SystemClass()
{
	Input = nullptr;
	Graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	bool Result = false;

	int ScreenWidth = 0;
	int ScreenHeight = 0;
	InitializeWindows(ScreenWidth, ScreenHeight);

	Input = new InputClass;
	if (Input)
	{
		Input->Initialize();

		Graphics = new GraphicsClass;
		if (Graphics)
		{
			Result = Graphics->Initialize(ScreenWidth, ScreenHeight, WindowHandle);
		}
	}

	return Result;
}

void SystemClass::Shutdown()
{
	if (Graphics)
	{
		Graphics->Shutdown();
		delete Graphics;
		Graphics = nullptr;
	}
	if (Input)
	{
		delete Input;
		Input = nullptr;
	}
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG Message;
	ZeroMemory(&Message, sizeof(MSG));

	bool IsDone = false;
	while (IsDone == false)
	{
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		if ((Message.message == WM_QUIT) || (Frame() == false))
		{
			IsDone = true;
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
	case WM_KEYDOWN:
		Input->KeyDown(WParam);
		break;

	case WM_KEYUP:
		Input->KeyUp(WParam);
		break;

	default:
		Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
		break;
	}

	return Result;
}

bool SystemClass::Frame()
{
	bool Result = false;

	if (Input->IsKeyDown(VK_ESCAPE) == false)
	{
		Result = Graphics->Frame();
	}

	return Result;
}

void SystemClass::InitializeWindows(int& ScreenWidth, int& ScreenHeight)
{
	gSystemClass = this;
	InstanceHandle = GetModuleHandle(nullptr);
	ApplicationName = L"Engine";

	WNDCLASSEX WindowClassEx;
	WindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClassEx.lpfnWndProc = WndProc;
	WindowClassEx.cbClsExtra = 0;
	WindowClassEx.cbWndExtra = 0;
	WindowClassEx.hInstance = InstanceHandle;
	WindowClassEx.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	WindowClassEx.hIconSm = WindowClassEx.hIcon;
	WindowClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClassEx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	WindowClassEx.lpszMenuName = nullptr;
	WindowClassEx.lpszClassName = ApplicationName;
	WindowClassEx.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&WindowClassEx);

	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int PositionX = 0;
	int PositionY = 0;
	if (IS_FULL_SCREEN)
	{
		DEVMODE DeviceDisplayMode;
		memset(&DeviceDisplayMode, 0, sizeof(DEVMODE));
		DeviceDisplayMode.dmSize = sizeof(DEVMODE);
		DeviceDisplayMode.dmPelsWidth = ScreenWidth;
		DeviceDisplayMode.dmPelsHeight = ScreenHeight;
		DeviceDisplayMode.dmBitsPerPel = 32;
		DeviceDisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&DeviceDisplayMode, CDS_FULLSCREEN);
	}
	else
	{
		ScreenWidth = 800;
		ScreenHeight = 600;

		PositionX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		PositionY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;
	}

	WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, ApplicationName, ApplicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, PositionX, PositionY, ScreenWidth, ScreenHeight, nullptr, nullptr, InstanceHandle, nullptr);
	ShowWindow(WindowHandle, SW_SHOW);
	SetForegroundWindow(WindowHandle);
	SetFocus(WindowHandle);
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);
	if (IS_FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}
	DestroyWindow(WindowHandle);
	WindowHandle = nullptr;
	UnregisterClass(ApplicationName, InstanceHandle);
	InstanceHandle = nullptr;
	gSystemClass = nullptr;
}

LRESULT CALLBACK WndProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		Result = gSystemClass->MessageHandler(WindowHandle, Message, WParam, LParam);
		break;
	}

	return Result;
}