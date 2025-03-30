#include "Process.h"
#include "Render.h"

Process::Process()
{
	m_pRenderer = new RenderSystem;
}

Process::~Process()
{
	delete m_pRenderer;
}

void Process::Init()
{
	m_pRenderer->Init();
}

void Process::Update()
{
	while (true)
	{
		if (!m_pRenderer->Update()) break;
	}
}

void Process::Final()
{
	m_pRenderer->Final();
}
