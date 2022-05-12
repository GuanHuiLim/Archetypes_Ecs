#pragma once

namespace ecs
{

class ComponentManager
{
public:
	ComponentManager() = default;
	~ComponentManager() = default;

	template<typename T>
	bool isRegistered();

	template<typename T>
	void RegisterComponent();
private:
	

};

}