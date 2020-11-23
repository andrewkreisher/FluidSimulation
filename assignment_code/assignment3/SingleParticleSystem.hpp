#ifndef SINGLE_PARTICLE_SYSTEM_H_
#define SINGLE_PARTICLE_SYSTEM_H_

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"


namespace GLOO {
	class SingleParticleSystem {
	public:
		SingleParticleSystem();
		glm::vec3 ComputeTimeDerivative(glm::vec3, float time) const;
	};
}  // namespace GLOO
#endif