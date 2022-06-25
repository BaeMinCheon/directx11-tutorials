// This code from the sample; "Microsoft DirectX SDK (June 2010)/Samples/C++/Direct3D10/Tutorials/Tutorial02/Tutorial02.cpp"

#include <Windows.h>
#include <D3D10.h>
#include <D3DX10.h>
#include "resource.h"

struct SimpleVertex
{
	D3DXVECTOR3 Position;
};

HINSTANCE						gInstanceHandle				= nullptr;
HWND							gWindowHandle				= nullptr;
D3D10_DRIVER_TYPE				gDriverType					= D3D10_DRIVER_TYPE_NULL;
ID3D10Device*					gDevice						= nullptr;
IDXGISwapChain*					gSwapChain					= nullptr;
ID3D10RenderTargetView*			gRenderTargetView			= nullptr;
ID3D10Effect*					gEffect						= nullptr;
ID3D10EffectTechnique*			gTechnique					= nullptr;
ID3D10InputLayout*				gInputLayout				= nullptr;
ID3D10Buffer*					gBuffer						= nullptr;

HRESULT							InitWindow(HINSTANCE, int);
HRESULT							InitDevice();
void							CleanupDevice();
LRESULT CALLBACK				WndProc(HWND, UINT, WPARAM, LPARAM);
void							Render();

int WINAPI wWinMain(HINSTANCE CurrentInstanceHandle, HINSTANCE PreviousInstanceHandle, LPWSTR CommandLine, int ShowOption)
{
	int Result = 0;

	UNREFERENCED_PARAMETER(PreviousInstanceHandle);
	UNREFERENCED_PARAMETER(CommandLine);

	if (SUCCEEDED(InitWindow(CurrentInstanceHandle, ShowOption)))
	{
		if (FAILED(InitDevice()))
		{
			CleanupDevice();
		}
		else
		{
			MSG Message = { 0 };
			while (Message.message != WM_QUIT)
			{
				if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					Render();
				}
			}
			CleanupDevice();
			Result = static_cast<int>(Message.wParam);
		}
	}

	return Result;
}

HRESULT InitWindow(HINSTANCE InstanceHandle, int ShowOption)
{
	HRESULT Result = E_FAIL;

	WNDCLASSEX WindowClassEx;
	WindowClassEx.cbSize = sizeof(WNDCLASSEX);
	WindowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	WindowClassEx.lpfnWndProc = WndProc;
	WindowClassEx.cbClsExtra = 0;
	WindowClassEx.cbWndExtra = 0;
	WindowClassEx.hInstance = InstanceHandle;
	WindowClassEx.hIcon = LoadIcon(InstanceHandle, reinterpret_cast<LPCTSTR>(IDI_TUTORIAL1));
	WindowClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClassEx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	WindowClassEx.lpszMenuName = nullptr;
	WindowClassEx.lpszClassName = L"Tutorial 01";
	WindowClassEx.hIconSm = LoadIcon(WindowClassEx.hInstance, reinterpret_cast<LPCTSTR>(IDI_TUTORIAL1));
	
	if (RegisterClassEx(&WindowClassEx))
	{
		gInstanceHandle = InstanceHandle;
		RECT Size = { 0, 0, 640, 480 };
		AdjustWindowRect(&Size, WS_OVERLAPPEDWINDOW, false);
		const int Width = Size.right - Size.left;
		const int Height = Size.bottom - Size.top;
		gWindowHandle = CreateWindow(L"Tutorial 01", L"Direct3D 11 Tutorial : Chapter 01", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, nullptr, nullptr, InstanceHandle, nullptr);

		if (gWindowHandle)
		{
			ShowWindow(gWindowHandle, ShowOption);
			Result = S_OK;
		}
	}

	return Result;
}

HRESULT InitDevice()
{
	HRESULT Result = E_FAIL;

	RECT Size;
	GetClientRect(gWindowHandle, &Size);
	UINT Width = Size.right - Size.left;
	UINT Height = Size.bottom - Size.top;
	UINT CreateDeviceFlags = 0;
#ifdef _DEBUG
	CreateDeviceFlags != D3D10_CREATE_DEVICE_DEBUG;
#endif

	D3D10_DRIVER_TYPE DriverTypes[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT DriverTypeNumbers = sizeof(DriverTypes) / sizeof(DriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC SwapChainDescription;
	ZeroMemory(&SwapChainDescription, sizeof(SwapChainDescription));
	SwapChainDescription.BufferCount = 1;
	SwapChainDescription.BufferDesc.Width = Width;
	SwapChainDescription.BufferDesc.Height = Height;
	SwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescription.OutputWindow = gWindowHandle;
	SwapChainDescription.SampleDesc.Count = 1;
	SwapChainDescription.SampleDesc.Quality = 0;
	SwapChainDescription.Windowed = true;

	bool bSwapChainCreated = false;
	for (UINT Index = 0; Index < DriverTypeNumbers; ++Index)
	{
		gDriverType = DriverTypes[Index];
		if (SUCCEEDED(D3D10CreateDeviceAndSwapChain(nullptr, gDriverType, nullptr, CreateDeviceFlags, D3D10_SDK_VERSION, &SwapChainDescription, &gSwapChain, &gDevice)))
		{
			bSwapChainCreated = true;
			break;
		}
	}

	if (bSwapChainCreated)
	{
		ID3D10Texture2D* Buffer;
		if (SUCCEEDED(gSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<LPVOID*>(&Buffer))))
		{
			if (SUCCEEDED(gDevice->CreateRenderTargetView(Buffer, nullptr, &gRenderTargetView)))
			{
				Buffer->Release();
				gDevice->OMSetRenderTargets(1, &gRenderTargetView, nullptr);
				
				D3D10_VIEWPORT Viewport;
				Viewport.Width = Width;
				Viewport.Height = Height;
				Viewport.MinDepth = 0.0f;
				Viewport.MaxDepth = 1.0f;
				Viewport.TopLeftX = 0;
				Viewport.TopLeftY = 0;
				gDevice->RSSetViewports(1, &Viewport);

				DWORD ShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
				ShaderFlags != D3D10_SHADER_DEBUG;
#endif
				if (FAILED(D3DX10CreateEffectFromFile(L"Tutorial02.fx", nullptr, nullptr, "fx_4_0", ShaderFlags, 0, gDevice, nullptr, nullptr, &gEffect, nullptr, nullptr)))
				{
					MessageBox(nullptr, L"Failed to create an effect from the FX file. Please check the FX file's location.", L"Error", MB_OK);
				}
				else
				{
					gTechnique = gEffect->GetTechniqueByName("Render");

					D3D10_INPUT_ELEMENT_DESC Layouts[] =
					{
						{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
					};
					UINT LayoutNumbers = sizeof(Layouts) / sizeof(Layouts[0]);

					D3D10_PASS_DESC PassDescription;
					gTechnique->GetPassByIndex(0)->GetDesc(&PassDescription);
					if (SUCCEEDED(gDevice->CreateInputLayout(Layouts, LayoutNumbers, PassDescription.pIAInputSignature, PassDescription.IAInputSignatureSize, &gInputLayout)))
					{
						gDevice->IASetInputLayout(gInputLayout);

						SimpleVertex Vertices[] =
						{
							D3DXVECTOR3(+0.0f, +0.5f, +0.5f),
							D3DXVECTOR3(+0.5f, -0.5f, +0.5f),
							D3DXVECTOR3(-0.5f, -0.5f, +0.5f),
						};
						UINT VertexNumbers = sizeof(Vertices) / sizeof(Vertices[0]);

						D3D10_BUFFER_DESC BufferDescription;
						BufferDescription.Usage = D3D10_USAGE_DEFAULT;
						BufferDescription.ByteWidth = sizeof(SimpleVertex) * VertexNumbers;
						BufferDescription.BindFlags = D3D10_BIND_VERTEX_BUFFER;
						BufferDescription.CPUAccessFlags = 0;
						BufferDescription.MiscFlags = 0;
						
						D3D10_SUBRESOURCE_DATA SubData;
						SubData.pSysMem = Vertices;

						if (SUCCEEDED(gDevice->CreateBuffer(&BufferDescription, &SubData, &gBuffer)))
						{
							UINT Stride = sizeof(SimpleVertex);
							UINT Offset = 0;
							gDevice->IASetVertexBuffers(0, 1, &gBuffer, &Stride, &Offset);
							gDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

							Result = S_OK;
						}
					}
				}
			}
			else
			{
				Buffer->Release();
			}
		}
	}

	return Result;
}

void CleanupDevice()
{
	if (gDevice)
	{
		gDevice->ClearState();
	}
	if (gBuffer)
	{
		gBuffer->Release();
	}
	if (gInputLayout)
	{
		gInputLayout->Release();
	}
	if (gEffect)
	{
		gEffect->Release();
	}
	if (gRenderTargetView)
	{
		gRenderTargetView->Release();
	}
	if (gSwapChain)
	{
		gSwapChain->Release();
	}
	if (gDevice)
	{
		gDevice->Release();
	}
}

LRESULT CALLBACK WndProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	PAINTSTRUCT Paint;
	HDC DCHandle;

	switch (Message)
	{
	case WM_PAINT:
		DCHandle = BeginPaint(WindowHandle, &Paint);
		EndPaint(WindowHandle, &Paint);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
		break;
	}

	return Result;
}

void Render()
{
	float ColorAsRGBA[4] =
	{
		0.0f,
		0.125f,
		0.3f,
		1.0f,
	};	
	gDevice->ClearRenderTargetView(gRenderTargetView, ColorAsRGBA);

	D3D10_TECHNIQUE_DESC TechniqueDescription;
	gTechnique->GetDesc(&TechniqueDescription);
	for (UINT Index = 0; Index < TechniqueDescription.Passes; ++Index)
	{
		gTechnique->GetPassByIndex(Index)->Apply(0);
		gDevice->Draw(3, 0);
	}

	gSwapChain->Present(0, 0);
}