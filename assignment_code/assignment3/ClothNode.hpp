#ifndef CLOTH_NODE_H_
#define CLOTH_NODE_H_
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
	class ClothNode : public SceneNode {
	public:
		ClothNode(IntegratorType type, float step_size);
		void Update(double delta_time) override;
		void Setup();
		int IndexOf(int i, int j);

	private:
		ParticleState state_;
		std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator_;
		std::shared_ptr<VertexObject> sphere_mesh_;
		std::shared_ptr<VertexObject> lines_;
		std::shared_ptr<ShaderProgram> shader_;
		std::unique_ptr<PendulumSystem> system_;
		std::unique_ptr<IndexArray> indices_;
		ParticleState init_state_;
		double time_;
		float step_size_;
		int dimension_;

	};
} //namespace gloo
#endif