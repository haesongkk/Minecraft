#pragma once
class RenderSystem;
class Process
{
public:
	Process();
	~Process();

	void Init();
	void Update();
	void Final();

private:
	RenderSystem* m_pRenderer = nullptr;
};

