#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

namespace DSS {
	class AudioManager;
}

class DynamicSoundtrackDemoAppApp : public aie::Application {
public:

	DynamicSoundtrackDemoAppApp();
	virtual ~DynamicSoundtrackDemoAppApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	DSS::AudioManager* m_AudioManager;

};