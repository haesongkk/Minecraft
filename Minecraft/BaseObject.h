#pragma once
#include "Base.h"
class Component;
class BaseObject : public Base
{
	/*unordered_map<size_t, Component*> m_componentMap;
	void AddComponent(Component* )
	{

	}
	template <class T>
	T* GetComponent()
	{
		auto it = m_componentMap.find(typeid(T).hash_code());
		if (it != m_componentMap.end())
			return static_cast<T*>(it->second);
		return nullptr;
	}*/

};