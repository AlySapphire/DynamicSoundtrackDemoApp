#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace DSS {
	class AudioManager;
	struct EventData;
}

class Camera;
class CollisionSphere;

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
	void SetupCollisionSpheres();
	DSS::EventData CreateEvent(unsigned int p_Duration, bool p_Mute = false, bool p_Pause = false, bool p_Fade = false,
							   float fadeLevelStart = 0.0f, float fadeLevelEnd = 0.0f, bool p_Volume = false, float p_VolumeLevel = 0.0f,
							   bool p_ChannelGroup = false, const char* p_ChannelGroupName = "", bool p_ChannelGroupOverall = false,
							   int p_ChannelNumber = 1, bool p_ResetValues = false);

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	Camera* m_Camera;
	CollisionSphere* m_CameraCollider;

	std::vector<CollisionSphere> m_EventSpheres;

	DSS::AudioManager* m_AudioManager;

	std::vector<DSS::EventData> m_Events;

};