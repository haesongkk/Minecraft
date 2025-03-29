#pragma once
class Render;
class Process
{
public:
	Process();
	~Process();

	void Init() { }
	void Update();
	void Final() { }

private:
	Render* m_pRenderer = nullptr;
};

