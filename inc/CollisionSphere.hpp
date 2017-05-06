#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace DSS {
	struct EventData;
}

class CollisionSphere {
public:
	CollisionSphere(glm::vec3 p_Centre, float p_Radius);
	virtual ~CollisionSphere();

	void Update(float deltaTime);

	void CheckForCollisions(CollisionSphere & p_Other);

	const glm::vec3& GetCentre();
	float GetRadius();

	void SetCentre(const glm::vec3& p_Centre);
	void SetRadius(float p_Radius);

	void SetEventData(std::vector<DSS::EventData>& p_Events);

protected:

	glm::vec3 m_Centre;
	float m_Radius;
	bool m_Colliding = false;
	float m_Time = 0.0f;
	bool m_ShowEvents = false;

	std::vector<DSS::EventData> m_Events;

};
