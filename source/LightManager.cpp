#include "pch.h"
#include "LightManager.h"
LightManager* LightManager::s_instance{ nullptr };

void LightManager::AddLightToManager(BaseLight* light)
{
	m_lights.push_back(light);
}

LightManager* LightManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new LightManager();
	return s_instance;
}

BaseLight* LightManager::GetLight(const int& index) const
{
	return m_lights[index];
}

const size_t LightManager::GetLightCount() const
{
	return m_lights.size();
}

void LightManager::ToggleHardShadows()
{
	m_RenderHardShadow = !m_RenderHardShadow;
}


bool LightManager::IsHardShadowTrue()
{
	return m_RenderHardShadow;
}



LightManager::~LightManager()
{
	for (int i = 0; i < m_lights.size(); i++)
	{
		delete m_lights[i];
	}
}
