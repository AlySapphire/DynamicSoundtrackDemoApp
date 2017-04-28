#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace DSS {
	class AudioManager;
	struct EventData;
}

class Camera;

class DynamicSoundtrackDemoAppApp : public aie::Application {
public:

	DynamicSoundtrackDemoAppApp();
	virtual ~DynamicSoundtrackDemoAppApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void DrawGrid();

	void PopulateEventData();

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	Camera* m_Camera;

	DSS::AudioManager* m_AudioManager;

	std::vector<DSS::EventData> m_Events;

};