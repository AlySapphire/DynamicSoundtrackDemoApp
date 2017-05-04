#include "DynamicSoundtrackDemoAppApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AudioManager.hpp>
#include <imgui.h>
#include <CustomClasses/Camera.h>
#include <ChannelManager.hpp>
#include <ChannelGroupManager.hpp>
#include <Submixer.hpp>
#include <string>
#include "CollisionSphere.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

DynamicSoundtrackDemoAppApp::DynamicSoundtrackDemoAppApp() : m_Camera(nullptr), m_CameraCollider(nullptr) {
	
}

DynamicSoundtrackDemoAppApp::~DynamicSoundtrackDemoAppApp() {

	m_Camera = nullptr;
	m_CameraCollider = nullptr;

}

bool DynamicSoundtrackDemoAppApp::startup() {

	DSS::ChannelGroupManager* cgm = DSS::ChannelGroupManager::Instance();
	
	//Grab a handle to the audio manager and initialize it
	m_AudioManager = DSS::AudioManager::Instance();
	m_AudioManager->Init(32);

	//Add Audio
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-VOCALS.wav", true);
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-LEADS.wav", true);
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-KEYS.wav", true);
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-DRUMS.wav", true);
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-BASS.wav", true);
	m_AudioManager->AddAudio("audio/WANO_Stems/WANO-FULL-SFX.wav", true);
	//m_AudioManager->AddAudio("audio/Prodigy Babe.wav", true);
	//m_AudioManager->channelManager->ToggleChannelPause(0);
	//m_AudioManager->SetChannelLoopPoints(0, 3500, 6500, DSS::eTIME_MS);
	//m_AudioManager->ToggleChannelPause(0);

	//Add Channels to ChannelGroup
	unsigned int wanoChannels[] = { 0, 1, 2, 3, 4, 5 };
	cgm->CreateChannelGroup("WANO", wanoChannels, 6);

	//Populate our Event data
	PopulateEventData();

	//Create Camera and set it's transforms
	m_Camera = new Camera(this);
	m_Camera->SetPosition(glm::vec3(10, 5, 0));
	m_Camera->LookAt(glm::vec3(0.0f));

	m_CameraCollider = new CollisionSphere(m_Camera->GetPosition(), 0.1f);

	SetupCollisionSpheres();

	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	return true;
}

void DynamicSoundtrackDemoAppApp::shutdown() {

	Gizmos::destroy();

	//Shutdown the audio manager
	m_AudioManager->Shutdown();

	//Destroy camera
	if(m_Camera != nullptr)				delete m_Camera;
	//Destroy Colliders
	if(m_CameraCollider != nullptr)		delete m_CameraCollider;
	m_EventSpheres.clear();

}

void DynamicSoundtrackDemoAppApp::update(float deltaTime) {

	//Update the audio manager
	m_AudioManager->Update(deltaTime);

	//Update the camera
	m_Camera->Update(deltaTime);

	//Update Camera Collider
	m_CameraCollider->SetCentre(m_Camera->GetPosition());

	//Check collisions
	for(unsigned int i = 0; i < m_EventSpheres.size(); i++) {
		m_EventSpheres[i].CheckForCollisions(*m_CameraCollider);
		//iter.CheckForCollisions(*m_CameraCollider);
	}

	unsigned int ms = m_AudioManager->channelManager->GetChannelPlaybackPosition(0);

	DSS::ChannelGroupManager* cgm = DSS::ChannelGroupManager::Instance();

	DSS::Submixer* wanoMixer = cgm->GetSubmixer("WANO");

	std::string TrackNames[] = { "Vocals", "Leads", "Keys", "Drums", "Bass", "SFX" };

	//ImGui::Begin("Channel Control");
	//ImGui::Text("Time: %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100);
	//if(ImGui::Button("Toggle Pause"))
	//	m_AudioManager->channelManager->ToggleChannelPause(0);
	//if(ImGui::Button("Create HighPass Effect"))
	//	m_AudioManager->CreateTimedEvent(4000, DSS::eEVENT_HIGHPASS);
	//if(ImGui::Button("Create LowPass Effect"))
	//	m_AudioManager->CreateTimedEvent(4000, DSS::eEVENT_LOWPASS);
	//if(ImGui::Button("Create Echo Effect"))
	//	m_AudioManager->CreateTimedEvent(4000, DSS::eEVENT_ECHO);
	//if(ImGui::Button("Create Flange Effect"))
	//	m_AudioManager->CreateTimedEvent(4000, DSS::eEVENT_FLANGE);
	//ImGui::End();

	float volume = 0.0f;
	float pitch = 0.0f;

	ImGui::Begin("ChannelGroup Control");
	ImGui::Text("Time: %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100);
	if(ImGui::Button("Play ChannelGroup"))
		cgm->PlayChannelGroup("WANO");
	if(ImGui::Button("Pause ChannelGroup"))
		cgm->PauseChannelGroup("WANO");
	if(ImGui::Button("Run Test Event"))
		m_AudioManager->ActivateEvents(m_Events);
	for(auto iter = 0; iter < 6; iter++) {
		volume = wanoMixer->GetVolume(iter);
		pitch = wanoMixer->GetPitch(iter);
		if(ImGui::TreeNode(TrackNames[iter].c_str())) {
			if(ImGui::Button("Toggle Mute"))
				wanoMixer->ToggleChannelMute(iter);
			if(ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
				wanoMixer->SetVolume(iter, volume);
			ImGui::TreePop();
		}
	}
	ImGui::End();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void DynamicSoundtrackDemoAppApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	DrawGrid();

	Gizmos::addSphere(glm::vec3(5), 2.0f, 20, 20, glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
	Gizmos::addSphere(glm::vec3(5, 5, -5), 2.0f, 20, 20, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	Gizmos::addSphere(glm::vec3(-5, 5, 5), 2.0f, 20, 20, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::addSphere(glm::vec3(-5, 5, -5), 2.0f, 20, 20, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::addSphere(glm::vec3(0, 5, 0), 2.0f, 20, 20, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

	// update perspective based on screen size
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);

	Gizmos::draw(m_projectionMatrix * m_Camera->GetView());
}

void DynamicSoundtrackDemoAppApp::DrawGrid() {

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for(int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

}

void DynamicSoundtrackDemoAppApp::PopulateEventData() {

	//Create a test event
	DSS::EventData testEvent;
	
	//Populate it with data
	//Use "WANO" channelgroup
	testEvent.channelGroup = true;
	testEvent.channelGroupName = "WANO";
	//Don't use the whole channel group but use an individual channel
	testEvent.channelGroupOverall = false;
	testEvent.channelNumber = 0;
	//Add a DSP filter
	testEvent.DSPType = DSS::eDSP_FLANGE;
	//Reset the values after use
	testEvent.resetValues = true;
	//Make Event last for 5 seconds/5000 milliseconds
	testEvent.duration = 5000;

	//Add our testEvent to our array
	m_Events.push_back(testEvent);

	//Create a test event
	DSS::EventData testEvent1;

	//Populate it with data
	//Use "WANO" channelgroup
	testEvent1.channelGroup = true;
	testEvent1.channelGroupName = "WANO";
	//Don't use the whole channel group but use an individual channel
	testEvent1.channelGroupOverall = false;
	testEvent1.channelNumber = 1;
	//Add a DSP filter
	testEvent1.DSPType = DSS::eDSP_HIGHPASS;
	//Reset the values after use
	testEvent1.resetValues = true;
	//Make Event last for 6 seconds/6000 milliseconds
	testEvent1.duration = 6000;

	//Add to array
	m_Events.push_back(testEvent1);

}

void DynamicSoundtrackDemoAppApp::SetupCollisionSpheres() {

	//Setup 4 spheres with their own radiuses, centre's and event data
	glm::vec3 centres[] = { glm::vec3(5), glm::vec3(5, 5, -5), glm::vec3(-5, 5, 5), glm::vec3(-5, 5, -5), glm::vec3(0, 5, 0) };

	std::vector<DSS::EventData> event1;

	event1.push_back(CreateEvent(5000, true, false, false, 0.0f, 0.0f, false, 0.0f, true, "WANO", false, 1, true));
	event1.push_back(CreateEvent(5500, false, false, false, 0.0f, 0.0f, false, 0.0f, true, "WANO", false, 0, true));
	event1.back().DSPType = DSS::eDSP_LOWPASS;
	event1.push_back(CreateEvent(4000, false, false, true, 1.0f, 5.0f, false, 0.0f, true, "WANO", false, 2, true));
	event1.push_back(CreateEvent(6000, false, false, false, 0.0f, 0.0f, true, 0.5f, true, "WANO", false, 3, true));
	event1.back().origVolume = 1.0f;

	std::vector<DSS::EventData> event2;

	event2.push_back(CreateEvent(6000, false, false, false, 0.0f, 0.0f, false, 0.0f, true, "WANO", true, 0, true));
	event2.back().DSPType = DSS::eDSP_ECHO;


	std::vector<DSS::EventData> event3;

	event3.push_back(CreateEvent(5000, false, false, true, 1.0f, 0.0f, false, 0.0f, true, "WANO", true, 0, true));

	std::vector<DSS::EventData> event4;

	event4.push_back(CreateEvent(6000, false, false, false, 0, 0, false, 0, true, "WANO", false, 0, true));
	event4.back().DSPType = DSS::eDSP_PITCH;
	DSS::DSP_Parameter pitchParams;
	pitchParams.dataType = DSS::eDSP_PARAMETER_VALUE_TYPE_FLOAT;
	pitchParams.parameterIndex = 0;
	pitchParams.value.floatTypeArgument = 0.5f;
	event4.back().DSP_Parameters.push_back(pitchParams);
	event4.push_back(CreateEvent(6000, true, false, false, 0, 0, false, 0, true, "WANO", false, 4, true));
	event4.push_back(CreateEvent(6000, false, false, true, 1.0f, 0.25f, false, 0, true, "WANO", false, 3, true));
	event4.push_back(CreateEvent(6000, false, false, false, 0, 0, false, 0, true, "WANO", false, 2, true));
	event4.back().DSPType = DSS::eDSP_FLANGE;
	event4.push_back(CreateEvent(6000, false, false, false, 0, 0, false, 0, true, "WANO", false, 1, true));
	event4.back().DSPType = DSS::eDSP_HIGHPASS;

	std::vector<DSS::EventData> event5;

	event5.push_back(CreateEvent(10000, false, false, false, 0, 0, false, 0, true, "WANO", true, 1, true));
	event5.back().DSPType = DSS::eDSP_PITCH;
	DSS::DSP_Parameter pitchParams1;
	pitchParams1.dataType = DSS::eDSP_PARAMETER_VALUE_TYPE_FLOAT;
	pitchParams1.parameterIndex = 0;
	pitchParams1.value.floatTypeArgument = 2.0f;
	event5.back().DSP_Parameters.push_back(pitchParams1);

	std::vector<DSS::EventData> events[] = { event1, event2, event3, event4, event5 };

	for(auto iter = 0; iter < 5; iter++) {

		CollisionSphere sphere(centres[iter], 2.0f);
		sphere.SetEventData(events[iter]);
		m_EventSpheres.push_back(sphere);

	}

}

DSS::EventData DynamicSoundtrackDemoAppApp::CreateEvent(unsigned int p_Duration, bool p_Mute, bool p_Pause, bool p_Fade, float fadeLevelStart, 
														float fadeLevelEnd, bool p_Volume, float p_VolumeLevel, bool p_ChannelGroup, 
														const char * p_ChannelGroupName, bool p_ChannelGroupOverall, int p_ChannelNumber, 
														bool p_ResetValues) {
	
	DSS::EventData temp;

	//Store values passed in
	temp.duration = p_Duration;
	temp.mute = p_Mute;
	temp.pause = p_Pause;
	temp.fade = p_Fade; 
	temp.fadeLevelStart = fadeLevelStart;
	temp.fadeLevelEnd = fadeLevelEnd;
	temp.volume = p_Volume;
	temp.volumeLevel = p_VolumeLevel;
	temp.channelGroup = p_ChannelGroup;
	temp.channelGroupName = p_ChannelGroupName;
	temp.channelGroupOverall = p_ChannelGroupOverall;
	temp.channelNumber = p_ChannelNumber;
	temp.resetValues = p_ResetValues;
	
	return temp;
}
