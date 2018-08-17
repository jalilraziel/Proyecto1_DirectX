
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dcompiler.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
        
ID3D11Device *dev;                     // puntero al dispositico del direcx_3D
ID3D11DeviceContext *devcon;           // puntero al contexto del dispositivo Direct3D
ID3D11RenderTargetView *pbackbuffer;   // puntero a la direccion del backbuffer
IDXGISwapChain *p_cadena;              // puntero a la interfaz de swapchain
ID3D11VertexShader *pvertex_S;         // puntero a la direccion del vertexshader
ID3D11PixelShader *ppixel_S;           // puntero a la direccion del pixelshader
ID3D11Buffer *pVertex_B;               // puntero a la direccion del vertexbuffer
ID3D11InputLayout *pLayout;            // puntero al dise?o de entrada


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Inicio_DXD3(HWND hWnd);
void RenderFrame();
void Limpiar_DXD3();
void Creando_Graphics();
void Creando_Pipeline();

struct Vertices{
	float X, Y, Z;
	D3DXCOLOR Color;
};

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX My_Ventana;
	ZeroMemory(&My_Ventana, sizeof(WNDCLASSEX));
	
	My_Ventana.cbSize = sizeof(WNDCLASSEX);
	My_Ventana.style = CS_HREDRAW | CS_VREDRAW;
	My_Ventana.lpfnWndProc = WindowProc;
	My_Ventana.cbClsExtra = 0;
	My_Ventana.cbWndExtra = 0;
	My_Ventana.hInstance = hInstance;
	My_Ventana.hIcon = NULL;
	My_Ventana.hCursor = LoadCursor(NULL, IDC_ARROW);
	My_Ventana.hbrBackground = (HBRUSH)COLOR_WINDOW;
	My_Ventana.lpszMenuName = NULL;
	My_Ventana.lpszClassName = L"ClaseVentana1";
	My_Ventana.hIconSm = NULL;
	if (!RegisterClassEx(&My_Ventana)) { 
		return E_FAIL; };
	RECT w_t = {0,0,720,480};
	AdjustWindowRect(&w_t, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(NULL, L"ClaseVentana1", L"proyecto 1", WS_OVERLAPPEDWINDOW, 300, 300, w_t.right-w_t.left, w_t.bottom-w_t.top, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	if (!&hWnd) {
		return E_FAIL;
	};
	Inicio_DXD3(hWnd);
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		RenderFrame();
	}
	Limpiar_DXD3();
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
		PAINTSTRUCT ps;
		HDC hdc;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Inicio_DXD3(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC cad_interc;
	ZeroMemory(&cad_interc, sizeof(DXGI_SWAP_CHAIN_DESC));

	cad_interc.BufferCount = 1;
	cad_interc.BufferDesc.Width = 720;;
	cad_interc.BufferDesc.Height = 480;;
	cad_interc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cad_interc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	cad_interc.OutputWindow = hWnd;
	cad_interc.SampleDesc.Count = 2;
	cad_interc.Windowed = TRUE;
											
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &cad_interc, &p_cadena, &dev, NULL, &devcon);

	ID3D11Texture2D *pback_B;
	p_cadena->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pback_B);
	dev->CreateRenderTargetView(pback_B, NULL, &pbackbuffer);
	pback_B->Release();
	devcon->OMSetRenderTargets(1, &pbackbuffer, NULL);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 720;
	viewport.Height = 480;
	devcon->RSSetViewports(1, &viewport);

	Creando_Pipeline();
	Creando_Graphics();
}

void Limpiar_DXD3()
{
	pLayout->Release();
	pvertex_S->Release();
	ppixel_S->Release();
	pVertex_B->Release();
	p_cadena->Release();
	pbackbuffer->Release();
	dev->Release();
	devcon->Release();
}

void RenderFrame(void)
{
	devcon->ClearRenderTargetView(pbackbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
	
	UINT size = sizeof(Vertices);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVertex_B, &size, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->Draw(3, 0);
	p_cadena->Present(0, 0);
}

void Creando_Pipeline()
{
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pvertex_S);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &ppixel_S);

	devcon->VSSetShader(pvertex_S, 0, 0);
	devcon->PSSetShader(ppixel_S, 0, 0);

	D3D11_INPUT_ELEMENT_DESC input_E_D[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(input_E_D, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}	


void Creando_Graphics()
{
	Vertices traingulo[] ={
		{ 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC buffer_d;
	ZeroMemory(&buffer_d, sizeof(buffer_d));

	buffer_d.Usage = D3D11_USAGE_DYNAMIC;
	buffer_d.ByteWidth = sizeof(Vertices) * 3;
	buffer_d.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&buffer_d, NULL, &pVertex_B);

	D3D11_MAPPED_SUBRESOURCE map_S;
	devcon->Map(pVertex_B, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &map_S);
	memcpy(map_S.pData, traingulo, sizeof(traingulo));
	devcon->Unmap(pVertex_B, NULL);

}