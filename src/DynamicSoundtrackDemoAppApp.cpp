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

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

DynamicSoundtrackDemoAppApp::DynamicSoundtrackDemoAppApp() : m_Camera(nullptr) {
	
}

DynamicSoundtrackDemoAppApp::~DynamicSoundtrackDemoAppApp() {

	m_Camera = nullptr;

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
	m_Camera->SetPosition(glm::vec3(1.0f));
	m_Camera->LookAt(glm::vec3(0.0f));

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
	if(m_Camera != nullptr)		delete m_Camera;

}

void DynamicSoundtrackDemoAppApp::update(float deltaTime) {

	//Update the audio manager
	m_AudioManager->Update(deltaTime);

	//Update the camera
	m_Camera->Update(deltaTime);

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
			//if(ImGui::SliderFloat("Pitch", &pitch, 0.0f, 2.0f))
			//	wanoMixer->SetPitch(iter, pitch);
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
