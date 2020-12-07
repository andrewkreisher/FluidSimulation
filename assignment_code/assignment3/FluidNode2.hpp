#ifndef FLUID2_NODE_H_
#define FLUID2_NODE_H_
#include <string>
#include <vector>
#include "gloo/SceneNode.hpp"
#include "IntegratorBase.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "PendulumSystem.hpp"
#include "FluidSystem.hpp"
#include "IntegratorType.hpp"


namespace GLOO {
	class FluidNode2 : public SceneNode {
	public:
		FluidNode2(IntegratorType type, float step_size);
		void Update(double delta_time) override;
		void Setup();
		void drawBox(bool filled);
		int IndexOf(int i, int j, int z);

	private:
		ParticleState state_;
		std::unique_ptr<IntegratorBase<FluidSystem, ParticleState>> integrator_;
		std::shared_ptr<VertexObject> sphere_mesh_;
		std::shared_ptr<VertexObject> lines_;
		std::shared_ptr<ShaderProgram> shader_;
		std::unique_ptr<FluidSystem> system_;
		std::unique_ptr<IndexArray> indices_;
		ParticleState init_state_;
		double time_;
		float step_size_;
		int dimension_;
		int floor_w_;
		float box_dim_;

	};
} //namespace gloo
#endif