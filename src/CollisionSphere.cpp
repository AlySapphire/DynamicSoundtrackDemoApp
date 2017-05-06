#include "CollisionSphere.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AudioManager.hpp>
#include <iostream>
#include <imgui.h>
#include <algorithm>
#include <glm/glm.hpp>

typedef unsigned int uint;

bool sortEvents(DSS::EventData a, DSS::EventData b) {
	return (a.duration > b.duration);
}

CollisionSphere::CollisionSphere(glm::vec3 p_Centre, float p_Radius) : m_Centre(p_Centre), m_Radius(p_Radius) {

}

CollisionSphere::~CollisionSphere() {

}

void CollisionSphere::Update(float deltaTime) {

	m_Time += deltaTime;

	if(m_ShowEvents) {
		char name[100];
		sprintf(name, "Sphere %u Events", sphereID);
		ImGui::Begin(name);
		for(uint i = 0; i < m_Events.size(); i++) {
			if(i == 0 && m_Time >= (float)(m_Events[i].duration / 1000)) {
				m_ShowEvents = false;
				break;
			}

			float timeRemaining = (float)(m_Events[i].duration / 1000) - m_Time;
			if(timeRemaining <= 0.0f)	break;
			float progress = ( timeRemaining / ((float)(m_Events[i].duration) / 1000));

			ImGui::Text("Event %u duration", i);
			ImGui::ProgressBar(progress);

		}
		ImGui::End();

	}

}

void CollisionSphere::CheckForCollisions(CollisionSphere & p_Other) {

	//Calculate distance vector
	glm::vec3 distVect = m_Centre - p_Other.GetCentre();

	//Calculate distance
	float distance = glm::length(distVect);

	//Get the sum of the radiuses
	float sumRadius = m_Radius + p_Other.GetRadius();

	//Work out our collision status and what to do
	if(distance < sumRadius) {
		if(!m_Colliding) {
			DSS::AudioManager::Instance()->ActivateEvents(m_Events);
			m_Colliding = true;
			std::cout << "Collided!" << std::endl;
			m_ShowEvents = true;
			m_Time = 0.0f;
		}
	} else {
		if(m_Colliding) {
			m_Colliding = false;
			std::cout << "Left collision area!" << std::endl;
		}
	}


}

const glm::vec3 & CollisionSphere::GetCentre() {
	return m_Centre;
}

float CollisionSphere::GetRadius() {
	return m_Radius;
}

void CollisionSphere::SetCentre(const glm::vec3 & p_Centre) {
	m_Centre = p_Centre;
}

void CollisionSphere::SetRadius(float p_Radius) {
	m_Radius = p_Radius;
}

void CollisionSphere::SetEventData(std::vector<DSS::EventData>& p_Events) {

	m_Events = p_Events;

	std::stable_sort(m_Events.begin(), m_Events.end(), sortEvents);

}

