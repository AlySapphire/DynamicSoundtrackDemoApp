#include "DynamicSoundtrackDemoAppApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AudioManager.hpp>
#include <imgui.h>
#include <CustomClasses/Camera.h>

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
	
	//Grab a handle to the audio manager and initialize it
	m_AudioManager = DSS::AudioManager::Instance();
	m_AudioManager->Init(32);

	//Add Audio and play it
	m_AudioManager->AddAudio("audio/Prodigy Babe.wav", true, 2);
	m_AudioManager->ToggleChannelPause(0);
	m_AudioManager->SetChannelLoopPoints(0, 3500, 6500, DSS::eTIME_MS);

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

	ImGui::Begin("Channel Control");
	if(ImGui::Button("Toggle Pause"))
		m_AudioManager->ToggleChannelPause(0);
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
