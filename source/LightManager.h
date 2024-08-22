#pragma once
#include "BaseLight.h"
#include <vector>
class LightManager
{
public:
	~LightManager();
	void AddLightToManager(BaseLight* light);
	static LightManager* GetInstance();
	BaseLight* GetLight(const int& index) const;
	const size_t GetLightCount() const;
	void ToggleHardShadows();
	bool IsHardShadowTrue();

private:
	LightManager() {};
	static LightManager* s_instance;
	std::vector<BaseLight*> m_lights;
	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;

	bool m_RenderHardShadow{ true };
};
