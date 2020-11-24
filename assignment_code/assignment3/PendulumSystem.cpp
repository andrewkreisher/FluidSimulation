#include "PendulumSystem.hpp"
#include "glm/gtx/string_cast.hpp"

namespace GLOO{

	PendulumSystem::PendulumSystem() {
		g_ = 9.81;
		vk_ = 0.2; 
		sk_ = 32; 
		sr_ = 0.2;		
	}

	ParticleState PendulumSystem::ComputeTimeDerivative(const ParticleState& state,
		float time) const {
		ParticleState deriv; 
		for (int i = 0; i < masses_.size(); i++) {
			if (!fixed_[i]) {
				float m = masses_[i];
				glm::vec3 x_i = state.positions[i];
				glm::vec3 vel = state.velocities[i];
				glm::vec3 gravity; 
				if (x_i[1] <= 0.5) {
					gravity = { 0,0.1 * m,0 };
				}
				else {
					gravity = { 0.0,  -0.1 * m , 0 };
				}
				
				glm::vec3 viscous_drag = -vk_ * vel; 
				glm::vec3 spring_force = { 0.f, 0.f, 0.f };
				std::vector<int> springs = springs_.at(i); 
				for (int j = 0; j < springs.size(); j++) {
					glm::vec3 x_j = state.positions[springs[j]]; 
					glm::vec3 d = x_i - x_j;
					float d_mag = glm::sqrt(glm::dot(d, d)); 
					glm::vec3 s_ij = -sk_ * (d_mag - sr_) * d / d_mag;
					spring_force += s_ij;
				}
				glm::vec3 A = (gravity + viscous_drag + spring_force) / m;;
				
				
				deriv.positions.push_back(vel); 
				deriv.velocities.push_back(A); 
				//std::cout << glm::to_string(viscous_drag) << std::endl;

			}
			else {
				deriv.positions.push_back({ 0.f, 0.f, 0.f });
				deriv.velocities.push_back({ 0.f, 0.f, 0.f });
			}
		}
		return deriv; 
	
	}

	void PendulumSystem::AddParticle(float mass) {
		masses_.push_back(mass); 
		std::vector<int> empty;
		springs_.insert({ masses_.size()-1, empty });
		fixed_.push_back(false);	
	}

	void PendulumSystem::AddSpring(int index1, int index2) {
		springs_[index1].push_back(index2); 
		springs_[index2].push_back(index1);
	}

	void PendulumSystem::FixPositionForParticle(int index) {
		fixed_[index] = true;
	}

}