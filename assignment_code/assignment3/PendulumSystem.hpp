#ifndef PENDULUM_SYSTEM_H_
#define PENDULUM_SYSTEM_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "ParticleSystemBase.hpp"

#include <string>
#include <vector>

namespace GLOO {
	class PendulumSystem : public ParticleSystemBase {
		public:
			PendulumSystem(); 
			ParticleState ComputeTimeDerivative(const ParticleState& state,
				float time) const;
			void AddParticle(float mass); 
			void AddSpring(int index1, int index2);
			void FixPositionForParticle(int index); 

		private:
			std::vector<float> masses_;
			std::vector<bool> fixed_; 
			std::unordered_map<int, std::vector<int>> springs_;
			ParticleState state_;
			float g_; 
			float vk_;
			float sk_; 
			float sr_;

	};
}
#endif