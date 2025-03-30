#include "Render.h"
#include "HSDX.h"

RenderSystem::RenderSystem()
{
    m_pHsdx = new HSDX;
}

RenderSystem::~RenderSystem()
{
    delete m_pHsdx;
}

void RenderSystem::Init()
{
    m_pHsdx->RegisterWindowClass();
    m_pHsdx->CreateWindowHandle();
    m_pHsdx->CreateDeviceAndSwapChain();
    m_pHsdx->CreateRenderTarget();
    m_pHsdx->CreateViewport();
    m_pHsdx->CreateDepthStencil();
    m_pHsdx->CreateBlendState();
    m_pHsdx->CreateShaderAndInputLayout();
    m_pHsdx->CreateBuffer();
    m_pHsdx->CreateSampler();
}

bool RenderSystem::Update()
{
    m_pHsdx->PresentFrame();
    m_pHsdx->ClearBuffers();
    m_pHsdx->SetupPipeline();
    m_pHsdx->UpdateCamera();
    m_pHsdx->UpdateConstantBuffers();
    m_pHsdx->BindShaders();
    return m_pHsdx->ProcessMessage();
}

void RenderSystem::Final()
{
    m_pHsdx->ReleaseAll();
}

