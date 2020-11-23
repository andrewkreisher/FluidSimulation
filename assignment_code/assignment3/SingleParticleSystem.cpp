#include "SingleParticleSystem.hpp"
namespace GLOO {

	SingleParticleSystem::SingleParticleSystem() {};

	glm::vec3 SingleParticleSystem::ComputeTimeDerivative(glm::vec3 state, float time) const {
		glm::vec3 new_pos = { -state[1], state[0], 0 };
		return new_pos;
	}
} //namespace