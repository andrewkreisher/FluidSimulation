
#ifndef PENDULUM_NODE_H_
#define PENDULUM_NODE_H_
#include <string>
#include <vector>
#include "gloo/SceneNode.hpp"
#include "IntegratorBase.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "PendulumSystem.hpp"
#include "IntegratorType.hpp"


namespace GLOO {
	class PendulumNode : public SceneNode {
	public:
		PendulumNode(IntegratorType type, float step_size);
		void Update(double delta_time) override;
		void Setup(); 

	private:
		ParticleState state_;
		std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator_;
		std::shared_ptr<VertexObject> sphere_mesh_;
		std::shared_ptr<ShaderProgram> shader_;
		std::unique_ptr<PendulumSystem> system_;
	    ParticleState init_state_;
		double time_;
		float step_size_;

	};
} //namespace gloo
#endif