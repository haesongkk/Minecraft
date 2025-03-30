#pragma once
class Base
{
public:
	Base() = default;
	virtual ~Base() = default;
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Final() = 0;
};