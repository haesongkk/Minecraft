#pragma once
class RenderComponent
{
public:

};
class HSDX;
class RenderSystem
{
public:
	RenderSystem();
	~RenderSystem();

	void Init();
	bool Update();
	void Final();

private:
	RenderComponent* m_arrComp = nullptr;

private:
	HSDX* m_pHsdx = nullptr;
};
using Render = RenderComponent;