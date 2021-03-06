#define _USE_MATH_DEFINES
#include "FluidSystem.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <vector>

namespace GLOO {

	FluidSystem::FluidSystem() {
		h_ = 0.2; 
		//h_ = 0.5;
		mu_ = 2;
		rest_density_ = 1000;
		pk_ = 2;
		restitution_ = 0.5;
	}

	ParticleState FluidSystem::ComputeTimeDerivative(const ParticleState& state,
		float time) const {
		ParticleState deriv;

		std::vector<float> densities; 
		std::vector<float> pressures;
		//compute densities && pressures

		for (int i = 0; i < masses_.size(); i++) {
			float density_i = 0;
			glm::vec3 r_i = state.positions.at(i);
			for (int j = 0; j < masses_.size(); j++) {
				glm::vec3 r_j = state.positions.at(j);
				float dens = masses_[j] * W(r_i - r_j, h_);
				density_i += dens;
			}
			pressures.push_back(pk_ * (density_i - rest_density_));
			densities.push_back(density_i);
		}

		//forces and acc
		for (int i = 0; i < masses_.size(); i++) {

				float m = masses_[i];
				glm::vec3 r_i = state.positions[i];
				glm::vec3 vel_i = state.velocities[i];
				glm::vec3 gravity = { 0, -9.81 * m, 0 };
				glm::vec3 pressure = glm::vec3(0.0); 
				glm::vec3 viscosity = glm::vec3(0.0);


				for (int j = 0; j < masses_.size(); j++) {
					if (i != j) {
						glm::vec3 r_j = state.positions[j];
						if (true) {
							glm::vec3 vel_j = state.velocities[j];
							
							float m_j = masses_[j];

							pressure += m_j * (pressures[i] / pow(densities[i], 2) + pressures[j] / pow(densities[j], 2)) * grad_W(r_i - r_j, h_);

							viscosity += m_j * ((vel_j - vel_i) / densities[j]) * laplace_W(r_i - r_j, h_);
						}
					}
				}
				glm::vec3 A = gravity + pressure + (mu_ / densities[i]) * viscosity;

				deriv.positions.push_back(vel_i);
				deriv.velocities.push_back(A);
		}
		
		return deriv;
	}


	void FluidSystem::AddParticle(float mass) {
		masses_.push_back(mass);
	}

	void FluidSystem::ComputeDensities(const ParticleState& state) {
		densities_.clear();
		for (int i = 0; i < masses_.size(); i++) {
			float density_i = 0;
			glm::vec3 r_i = state.positions.at(i);
			for (int j = 0; j < masses_.size(); j++) {
				glm::vec3 r_j = state.positions.at(j);
				float dens = masses_[j] * W(r_i - r_j, h_);
				density_i += dens;
			}
			densities_.push_back(density_i);
		}
	}

	float FluidSystem::W(glm::vec3 r, float h) const {
		float left  = 315.0f / (64.0f * M_PI * pow(h, 9));
		return left * pow((pow(h, 2) - pow(glm::length(r), 2)), 3);
	}

	glm::vec3 FluidSystem::grad_W(glm::vec3 r, float h) const {
		float left  = -45.0f / ( M_PI * pow(h, 6));
		return left * pow((h-glm::length(r)), 2) * glm::normalize(r);
	}

	float FluidSystem::laplace_W(glm::vec3 r, float h)  const {
		float left = 45.0f / (M_PI * pow(h, 6));
		return left * (h - length(r));
	}


}