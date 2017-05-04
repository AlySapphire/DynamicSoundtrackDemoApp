#include "CollisionSphere.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AudioManager.hpp>

CollisionSphere::CollisionSphere(glm::vec3 p_Centre, float p_Radius) : m_Centre(p_Centre), m_Radius(p_Radius) {

}

CollisionSphere::~CollisionSphere() {

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
		}
	} else {
		m_Colliding = false;
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

}

