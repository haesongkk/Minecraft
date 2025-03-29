
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/Simplemath.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>

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
#include "Process.h"
#include "Render.h"

Process::Process()
{
	m_pRenderer = new Render;
}

Process::~Process()
{
	delete m_pRenderer;
}

void Process::Update()
{
	m_pRenderer->Update();
}
