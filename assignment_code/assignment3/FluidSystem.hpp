#ifndef FLUID_SYSTEM_H_
#define FLUID_SYSTEM_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "ParticleSystemBase.hpp"

#include <string>
#include <vector>

namespace GLOO {
	class FluidSystem : public ParticleSystemBase {
	public:
		FluidSystem();
		ParticleState ComputeTimeDerivative(const ParticleState& state,
			float time) const;
		void AddParticle(float mass);
		void ComputeDensities(const ParticleState& state);
		float W(glm::vec3 r, float h); 
		glm::vec3 grad_W(glm::vec3 r, float h);
		float laplace_W(glm::vec3 r, float h);

	private:
		std::vector<float> masses_;
		std::vector<float> densities_;
		std::vector<bool> fixed_;
		std::unordered_map<int, std::vector<int>> springs_;
		ParticleState state_;
		float h_;
		float pk_;
		float mu_;
	};
}
#endif