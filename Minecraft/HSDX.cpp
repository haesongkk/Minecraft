#include <iostream>

#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/Simplemath.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include "HSDX.h"

class HSDX::IMPL
{
public:
    D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pDeviceContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;

    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    ID3D11DepthStencilView* pDepthStencilView = nullptr;
    ID3D11BlendState* m_pAlphaBlendState = nullptr;

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11SamplerState* m_pSamplerLinear = nullptr;

    ID3D11Buffer* m_pTransformBuffer = nullptr;
    ID3D11Buffer* m_pLightBuffer = nullptr;
    ID3D11Buffer* m_pMaterialBuffer = nullptr;
    ID3D11Buffer* m_pUsingBuffer = nullptr;
    ID3D11Buffer* m_pBonesBuffer = nullptr;

    HWND hWnd;
    int width;
    int height;
    bool bQuit = false;

    struct TransformBuffer
    {
        Matrix mWorld;
        Matrix mView;
        Matrix mProjection;
    } m_transform;

    struct LightBuffer
    {
        Vector3 Direction = { 0.3f, 0.0f, 0.6f };
        float pad0;
        Vector4 Ambient = { 0.1f,0.1f,0.1f,1.0f };
        Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
        Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
        Vector3 EyePosition;
        float pad1;
    } m_light;

    struct MarterialBuffer
    {
        Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
        Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
        Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
        float  SpecularPower = 200;
        Vector3 pad0;
    } m_material;

    struct UsingBuffer
    {
        bool UsingDiffuseMap = true;
        bool padding0[3];

        bool UsingNormalMap = true;
        bool padding1[3];

        bool UsingSpecularMap = true;
        bool padding2[3];

        bool UsingEmissiveMap = true;
        bool padding3[3];

        bool UsingOpacityMap = true;
        bool padding4[3];

        Vector3 padding5;
    } m_using;

    struct BoneBuffer
    {
        Matrix bonePallete[128] = {};
    } m_bones;

    struct Camera
    {
        Vector3 pos = { 0,100,-250 };
        Vector3 dir = { 0,0,1 };
        Vector3 headDir = { 0,1,0 };
        Matrix viewMatrix;

        float fovY = 1.570796327f;
        float nearZ = 0.1f;
        float farZ = 10000.0f;
        Matrix projMatrix;
    } m_camera;
};

HSDX::HSDX()
{
    m_pImpl = new IMPL;
}

void HSDX::RegisterWindowClass(const wchar_t* _windowID)
{
    WNDCLASSEX wcex = {};
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.lpfnWndProc = DefWindowProc;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpszClassName = _windowID;
    RegisterClassEx(&wcex);
}

void HSDX::CreateWindowHandle(int _width, int _height, int _leftTopX, int _leftTopY, bool _bTitle, const wchar_t* _windowTitle, const wchar_t* _windowID)
{
    m_pImpl->hWnd = CreateWindow(
        _windowID, _windowTitle,
        _bTitle ? WS_OVERLAPPEDWINDOW : WS_POPUP,
        _leftTopX, _leftTopY,
        _width + (_bTitle ? GetSystemMetrics(SM_CXFRAME) * 2 : 0),
        _height + (_bTitle ? GetSystemMetrics(SM_CXFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION) : 0),
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow(m_pImpl->hWnd, SW_SHOW);
    UpdateWindow(m_pImpl->hWnd); // 없애도 되면 없애기
}

void HSDX::CreateDeviceAndSwapChain(int _width, int _height, int _renderFormat, bool _bUseVsync, int _msaaCount, bool _bDebug, bool _bFullScreen)
{
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_pImpl->hWnd;
    swapDesc.Windowed = !_bFullScreen;
    swapDesc.BufferDesc.Format = (DXGI_FORMAT)_renderFormat;
    swapDesc.BufferDesc.Width = _width;
    swapDesc.BufferDesc.Height = _height;
    swapDesc.BufferDesc.RefreshRate.Numerator = _bUseVsync ? 60 : 0;
    swapDesc.BufferDesc.RefreshRate.Denominator = _bUseVsync ? 1 : 0;
    swapDesc.SampleDesc.Count = _msaaCount;
    swapDesc.SampleDesc.Quality = 0;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        _bDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &swapDesc,
        &m_pImpl->pSwapChain,
        &m_pImpl->pDevice,
        &m_pImpl->d3dFeatureLevel,
        &m_pImpl->pDeviceContext
    );
}

void HSDX::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBufferTexture = nullptr;
    m_pImpl->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
    m_pImpl->pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pImpl->pRenderTargetView);
    m_pImpl->pDeviceContext->OMSetRenderTargets(1, &m_pImpl->pRenderTargetView, NULL);
    pBackBufferTexture->Release();
}

void HSDX::CreateViewport(float _width, float _height)
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = _width;
    viewport.Height = _height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pImpl->pDeviceContext->RSSetViewports(1, &viewport);
}

void HSDX::CreateDepthStencil(float _width, float _height)
{
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = _width;
    descDepth.Height = _height;
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
    m_pImpl->pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil);
    assert(textureDepthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0;
    m_pImpl->pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pImpl->pDepthStencilView);

    textureDepthStencil->Release();

    m_pImpl->pDeviceContext->OMSetRenderTargets(1, &m_pImpl->pRenderTargetView, m_pImpl->pDepthStencilView);
}

void HSDX::CreateBlendState()
{
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

    m_pImpl->pDevice->CreateBlendState(&blendDesc, &m_pImpl->m_pAlphaBlendState);
}

void HSDX::CreateShaderAndInputLayout()
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DBlob* vertexShaderBuffer = nullptr;
    ID3DBlob* pixelShaderBuffer = nullptr;

    SetCurrentDirectoryW(L"D:\\Minecraft\\Run");


    // Vertex Shader
    D3DReadFileToBlob(L"./BasicVertexShader.cso", &vertexShaderBuffer);
    if (!vertexShaderBuffer)
    {
        D3DCompileFromFile(L"./BasicVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main", "vs_5_0", 0, 0, &vertexShaderBuffer, nullptr);
    }

    // Pixel Shader
    D3DReadFileToBlob(L"./BasicPixelShader.cso", &pixelShaderBuffer);
    if (!pixelShaderBuffer)
    {
        D3DCompileFromFile(L"./BasicPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main", "ps_5_0", 0, 0, &pixelShaderBuffer, nullptr);
    }

    m_pImpl->pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pImpl->m_pVertexShader);
    m_pImpl->pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pImpl->m_pPixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES" , 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHTS" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };



    m_pImpl->pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pImpl->m_pInputLayout);

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
}

void HSDX::CreateBuffer()
{
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(IMPL::TransformBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pImpl->pDevice->CreateBuffer(&cbDesc, nullptr, &m_pImpl->m_pTransformBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(IMPL::LightBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pImpl->pDevice->CreateBuffer(&cbDesc, nullptr, &m_pImpl->m_pLightBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(IMPL::MarterialBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pImpl->pDevice->CreateBuffer(&cbDesc, nullptr, &m_pImpl->m_pMaterialBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(IMPL::UsingBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pImpl->pDevice->CreateBuffer(&cbDesc, nullptr, &m_pImpl->m_pUsingBuffer);

    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(IMPL::BoneBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pImpl->pDevice->CreateBuffer(&cbDesc, nullptr, &m_pImpl->m_pBonesBuffer);
}

void HSDX::CreateSampler()
{
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_pImpl->pDevice->CreateSamplerState(&sampDesc, &m_pImpl->m_pSamplerLinear);
}

bool HSDX::ProcessMessage()
{
    MSG msg;
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
    DispatchMessage(&msg);
    return IsWindow(m_pImpl->hWnd);
}

void HSDX::PresentFrame()
{
    m_pImpl->pSwapChain->Present(0, 0);
}

void HSDX::ClearBuffers()
{
    float clearColor[4] = { 0.05f, 0.05f, 0.1f, 1.0f };
    m_pImpl->pDeviceContext->ClearRenderTargetView(m_pImpl->pRenderTargetView, clearColor);
    m_pImpl->pDeviceContext->ClearDepthStencilView(m_pImpl->pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void HSDX::SetupPipeline()
{
    m_pImpl->pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pImpl->pDeviceContext->IASetInputLayout(m_pImpl->m_pInputLayout);
}

void HSDX::UpdateCamera(float _width, float _height)
{
    m_pImpl->m_camera.viewMatrix = DirectX::XMMatrixLookToLH(m_pImpl->m_camera.pos, m_pImpl->m_camera.dir, m_pImpl->m_camera.headDir);
    m_pImpl->m_camera.projMatrix = DirectX::XMMatrixPerspectiveFovLH(m_pImpl->m_camera.fovY, _width / _height, m_pImpl->m_camera.nearZ, m_pImpl->m_camera.farZ);
    m_pImpl->m_transform.mView = XMMatrixTranspose(m_pImpl->m_camera.viewMatrix);
    m_pImpl->m_transform.mProjection = XMMatrixTranspose(m_pImpl->m_camera.projMatrix);
}
 
void HSDX::UpdateConstantBuffers()
{
    m_pImpl->m_light.EyePosition = m_pImpl->m_camera.pos;
    m_pImpl->pDeviceContext->UpdateSubresource(m_pImpl->m_pLightBuffer, 0, nullptr, &m_pImpl->m_light, 0, 0);
    m_pImpl->pDeviceContext->UpdateSubresource(m_pImpl->m_pTransformBuffer, 0, nullptr, &m_pImpl->m_transform, 0, 0);
}

void HSDX::BindShaders()
{
    m_pImpl->pDeviceContext->VSSetShader(m_pImpl->m_pVertexShader, nullptr, 0);
    m_pImpl->pDeviceContext->VSSetConstantBuffers(0, 1, &m_pImpl->m_pTransformBuffer);
    m_pImpl->pDeviceContext->VSSetConstantBuffers(1, 1, &m_pImpl->m_pLightBuffer);
    m_pImpl->pDeviceContext->PSSetShader(m_pImpl->m_pPixelShader, nullptr, 0);
    m_pImpl->pDeviceContext->PSSetConstantBuffers(0, 1, &m_pImpl->m_pTransformBuffer);
    m_pImpl->pDeviceContext->PSSetConstantBuffers(1, 1, &m_pImpl->m_pLightBuffer);
    m_pImpl->pDeviceContext->PSSetSamplers(0, 1, &m_pImpl->m_pSamplerLinear);
}

void HSDX::ReleaseAll()
{
    m_pImpl->m_pVertexShader->Release();
    m_pImpl->m_pPixelShader->Release();
    m_pImpl->m_pInputLayout->Release();
    m_pImpl->m_pTransformBuffer->Release();
    m_pImpl->pDevice->Release();
    m_pImpl->pDeviceContext->Release();
    m_pImpl->pSwapChain->Release();
    m_pImpl->pRenderTargetView->Release();
    m_pImpl->pDepthStencilView->Release();
}
