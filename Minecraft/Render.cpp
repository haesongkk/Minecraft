#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/Simplemath.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>

#include <wrl/client.h> // ComPtr 정의
using Microsoft::WRL::ComPtr;
#include <iostream>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#define WIDTH 1024
#define HEIGHT 768


#include "Render.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

Render::Render()
{
    // 윈도우 초기화
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wcex = {};
    wcex.hInstance = hInstance;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"DefaultWindowCalss";

    RegisterClassEx(&wcex);
    
    RECT rt = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindow(
        L"DefaultWindowCalss", 
        L"GameApp", 
        WS_OVERLAPPEDWINDOW,
        100, 100, rt.right - rt.left, rt.bottom - rt.top, 
        nullptr, 
        nullptr, 
        hInstance, 
        nullptr);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // DX 초기화
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hWnd;
    swapDesc.Windowed = true;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.Width = WIDTH;
    swapDesc.BufferDesc.Height = HEIGHT;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.SampleDesc.Count = 4;
    swapDesc.SampleDesc.Quality = 0;

    UINT creationFlags = 0;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
        D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
    assert(m_pSwapChain);
    assert(m_pDevice);
    assert(m_pDeviceContext);

    ID3D11Texture2D* pBackBufferTexture = nullptr;

    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
    assert(pBackBufferTexture);

    m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView);
    assert(m_pRenderTargetView);

    pBackBufferTexture->Release();

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)WIDTH;
    viewport.Height = (float)HEIGHT;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pDeviceContext->RSSetViewports(1, &viewport);

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = WIDTH;
    descDepth.Height = HEIGHT;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 4;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* textureDepthStencil = nullptr;
    m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil);
    assert(textureDepthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0;
    m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthStencilView);
    assert(m_pDepthStencilView);
    
    textureDepthStencil->Release();

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState);


    assert(m_pAlphaBlendState);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES" , 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHTS" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DBlob* vertexShaderBuffer = nullptr;
    ID3DBlob* pixelShaderBuffer = nullptr;

    WCHAR path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, path);

    // Wide 문자 → ANSI 문자열로 변환
    char ansiPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, path, -1, ansiPath, MAX_PATH, nullptr, nullptr);

    std::cout << "현재 작업 디렉토리: " << ansiPath << std::endl;

    SetCurrentDirectoryW(L"D:\\Minecraft\\Run");

    GetCurrentDirectoryW(MAX_PATH, path);
    WideCharToMultiByte(CP_ACP, 0, path, -1, ansiPath, MAX_PATH, nullptr, nullptr);
    std::cout << "변경된 작업 디렉토리: " << ansiPath << std::endl;

    HRESULT hr = S_OK;
    ComPtr<ID3DBlob> errorBlob;

    // Vertex Shader
    hr = D3DReadFileToBlob(L"./BasicVertexShader.cso", &vertexShaderBuffer);
    if (FAILED(hr) || !vertexShaderBuffer)
    {
        hr = D3DCompileFromFile(L"./BasicVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorBlob);
        if (FAILED(hr)) {
            if (errorBlob) {
                std::cerr << "Vertex Shader Error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
            }
            else {
                std::cerr << "Vertex Shader Compile Failed with no error message." << std::endl;
            }
            assert(false);
        }
    }

    // Pixel Shader
    hr = D3DReadFileToBlob(L"./BasicPixelShader.cso", &pixelShaderBuffer);
    if (FAILED(hr) || !pixelShaderBuffer)
    {
        hr = D3DCompileFromFile(L"./BasicPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorBlob);
        if (FAILED(hr)) {
            if (errorBlob) {
                std::cerr << "Pixel Shader Error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
            }
            else {
                std::cerr << "Pixel Shader Compile Failed with no error message." << std::endl;
            }
            assert(false);
        }
    }
    m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    assert(m_pVertexShader);

    m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
    assert(m_pInputLayout);

    m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
    assert(m_pPixelShader);

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(TransformBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pTransformBuffer);
    assert(m_pTransformBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(LightBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pLightBuffer);
    assert(m_pLightBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(MarterialBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pMaterialBuffer);
    assert(m_pMaterialBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(UsingBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pUsingBuffer);
    assert(m_pUsingBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(BoneBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pBonesBuffer);
    assert(m_pBonesBuffer);

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    assert(m_pSamplerLinear);

    // imgui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);
}

Render::~Render()
{

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_pVertexShader->Release();
    m_pPixelShader->Release();
    m_pInputLayout->Release();
    m_pTransformBuffer->Release();

    m_pDevice->Release();
    m_pDeviceContext->Release();
    m_pSwapChain->Release();
    m_pRenderTargetView->Release();
    m_pDepthStencilView->Release();

}

void Render::Update()
{
    while (TRUE)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            if (msg.message == WM_DESTROY) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            m_pSwapChain->Present(0, 0);

            m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, Color{ 0.05f, 0.05f, 0.1f, 1.0f });
            m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

            m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_pDeviceContext->IASetInputLayout(m_pInputLayout);

            m_camera.viewMatrix = DirectX::XMMatrixLookToLH(m_camera.pos, m_camera.dir, m_camera.headDir);
            m_camera.projMatrix = DirectX::XMMatrixPerspectiveFovLH(m_camera.fovY, WIDTH / (FLOAT)HEIGHT, m_camera.nearZ, m_camera.farZ);
            m_transform.mView = XMMatrixTranspose(m_camera.viewMatrix);
            m_transform.mProjection = XMMatrixTranspose(m_camera.projMatrix);

            m_light.EyePosition = m_camera.pos;

            m_pDeviceContext->UpdateSubresource(m_pLightBuffer, 0, nullptr, &m_light, 0, 0);
            m_pDeviceContext->UpdateSubresource(m_pTransformBuffer, 0, nullptr, &m_transform, 0, 0);

            m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
            m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pLightBuffer);
            m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTransformBuffer);

            m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
            m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pLightBuffer);
            m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTransformBuffer);

            m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(200, 150));

            ImGui::Begin("settings");
            ImGui::Text("rotate (radian)");
            //ImGui::DragFloat3("##rotate", (float*)&(m_pGameObjects[0]->m_rotate), 0.1f, -4.f, 4.f);
            ImGui::Text("camera");
            ImGui::DragFloat3("##camera", (float*)&(m_camera.pos), 1.f, -10000.f, 10000.f);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    }
}
