#pragma once
class HSDX
{
public:
    HSDX();

public:
    // 윈도우 클래스 등록
    void RegisterWindowClass(const wchar_t* _windowID = L"GameClass");

    // 윈도우 창 생성
    void CreateWindowHandle(
        int _width = 1024, int _height = 768,
        int _leftTopX = 100, int _leftTopY = 100,
        bool _bTitle = true,
        const wchar_t* _windowTitle = L"MineCraft",
        const wchar_t* _windowID = L"GameClass"
    );

    // D3D11 기본 환경 구축
    void CreateDeviceAndSwapChain(
        int _width = 1024, int _height = 768,
        int _renderFormat = 28,
        bool _bUseVsync = false,
        int _msaaCount = 4,
        bool _bDebug = false,
        bool _bFullScreen = false
    );

    void CreateRenderTarget();
    void CreateViewport(float _width = 1024, float _height = 768);
    void CreateDepthStencil(float _width = 1024, float _height = 768);

public:
    void CreateBlendState();
    void CreateShaderAndInputLayout();
    void CreateBuffer();
    void CreateSampler();

public:
    bool ProcessMessage();
    void PresentFrame();
    void ClearBuffers();
    void SetupPipeline();
    void UpdateCamera(float _width = 1024, float _height = 768);
    void UpdateConstantBuffers();
    void BindShaders();

public:
    void ReleaseAll();


private:
    class IMPL;
    IMPL* m_pImpl;
};

