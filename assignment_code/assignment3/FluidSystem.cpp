#define _USE_MATH_DEFINES
#include "FluidSystem.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>

namespace GLOO {

	FluidSystem::FluidSystem() {
		
	}

	ParticleState FluidSystem::ComputeTimeDerivative(const ParticleState& state,
		float time) const {
		ParticleState deriv;
		return deriv;
	}

	float FluidSystem::W(glm::vec3 r, float h) {
		float left  = 315.0f / (64.0f * M_PI * pow(h, 9));
		return left * pow((pow(h, 2) - pow(glm::length(r), 2)), 3);
	}

	glm::vec3 FluidSystem::grad_W(glm::vec3 r, float h) {
		float left  = -45.0f / ( M_PI * pow(h, 6));
		return left * pow((h-glm::length(r)), 2) * glm::normalize(r);
	}

	float FluidSystem::laplace_W(glm::vec3 r, float h) {
		float left = 45.0f / (M_PI * pow(h, 6));
		return left * (h - length(r));
	}

}