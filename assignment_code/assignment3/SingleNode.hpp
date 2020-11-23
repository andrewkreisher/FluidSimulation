
#ifndef SINGLE_NODE_H_
#define SINGLE_NODE_H_
#include <string>
#include <vector>
#include "gloo/SceneNode.hpp"
#include "IntegratorBase.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "SingleParticleSystem.hpp"


namespace GLOO {
	class SingleNode : public SceneNode {
	public:
		SingleNode(IntegratorType type, float step_size);
		void Update(double delta_time) override;

	private:
		glm::vec3 state_;
		std::unique_ptr<IntegratorBase<SingleParticleSystem, glm::vec3>> integrator_;
		std::shared_ptr<VertexObject> sphere_mesh_;
		std::shared_ptr<ShaderProgram> shader_;
		std::unique_ptr<SingleParticleSystem> system_; 
		double time_;
		float step_size_;

	};
} //namespace gloo
#endif