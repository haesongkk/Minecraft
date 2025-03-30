#pragma once
class HSDX
{
public:
    HSDX();

public:
    // ������ Ŭ���� ���
    void RegisterWindowClass(const wchar_t* _windowID = L"GameClass");

    // ������ â ����
    void CreateWindowHandle(
        int _width = 1024, int _height = 768,
        int _leftTopX = 100, int _leftTopY = 100,
        bool _bTitle = true,
        const wchar_t* _windowTitle = L"MineCraft",
        const wchar_t* _windowID = L"GameClass"
    );

    // D3D11 �⺻ ȯ�� ����
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

