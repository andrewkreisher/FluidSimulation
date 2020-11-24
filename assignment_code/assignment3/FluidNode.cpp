#include "FluidNode.hpp"
#include "PendulumNode.hpp"
#include "gloo/SceneNode.hpp"
#include "SingleParticleSystem.hpp"
#include "IntegratorBase.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "glm/gtx/string_cast.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "SingleNode.hpp"
#include "ParticleState.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/shaders/SimpleShader.hpp"



namespace GLOO {

	FluidNode::FluidNode(IntegratorType type, float step_size) {
		integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.010f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<PendulumSystem>();
		time_ = 0.0;
		step_size_ = step_size;
		dimension_ = 16;
		lines_ = std::make_shared<VertexObject>();
		Setup();
	};

	int FluidNode::IndexOf(int i, int j) {
		return dimension_ * j + i;
	}

	void FluidNode::Setup() {
		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				auto node = make_unique<SceneNode>();
				node->CreateComponent<ShadingComponent>(shader_);
				node->CreateComponent<RenderingComponent>(sphere_mesh_);
				//glm::vec3 pos = { 1 + 0.15 * j,  0,1 - 0.15 * i };
				glm::vec3 pos = { 1 + 0.15 * j,  1 - 0.15 * i , 0 };
				node->GetTransform().SetPosition(pos);
				system_->AddParticle(0.1);
				//positions->push_back(pos);
				state_.positions.push_back(pos);
				state_.velocities.push_back({ 0,0,0 });
				AddChild(std::move(node));
			}
		}
	}

}